#include "lambdamoduleconfiguration.h"
#include "lambdamoduleconfigdata.h"
#include <xmlconfigreader.h>

static void initResource()
{
    Q_INIT_RESOURCE(lambdamodulexmlschema);
}

namespace LAMBDAMODULE
{
static const char* defaultXSDFile = "://lambdamodule.xsd";

cLambdaModuleConfiguration::cLambdaModuleConfiguration()
{
    initResource();
    m_pLambdaModulConfigData = 0;
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::valueChanged, this, &cLambdaModuleConfiguration::configXMLInfo);
    connect(m_pXMLReader, &Zera::XMLConfig::cReader::finishedParsingXML, this, &cLambdaModuleConfiguration::completeConfiguration);
}


cLambdaModuleConfiguration::~cLambdaModuleConfiguration()
{
    if (m_pLambdaModulConfigData) delete m_pLambdaModulConfigData;
}


void cLambdaModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pLambdaModulConfigData) delete m_pLambdaModulConfigData;
    m_pLambdaModulConfigData = new cLambdaModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["lambdamodconfpar:configuration:measure:activepmeasmode:avail"] = setActivePMeasModeAvail;
    m_ConfigXMLMap["lambdamodconfpar:configuration:measure:activepmeasmode:inputentity"] = setActivePMeasModeEntity;
    m_ConfigXMLMap["lambdamodconfpar:configuration:measure:activepmeasmode:componentmeasmode"] = setActivePMeasModeComponent;
    m_ConfigXMLMap["lambdamodconfpar:configuration:measure:activepmeasmode:componentmeasmodephase"] = setActivePMeasModePhaseSelectComponent;
    m_ConfigXMLMap["lambdamodconfpar:configuration:measure:system:n"] = setSystemCount;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cLambdaModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cLambdaModuleConfigData *cLambdaModuleConfiguration::getConfigurationData()
{
    return m_pLambdaModulConfigData;
}


void cLambdaModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setActivePMeasModeAvail:
            m_pLambdaModulConfigData->m_activeMeasModeAvail = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        case setActivePMeasModeEntity:
            m_pLambdaModulConfigData->m_activeMeasModeEntity = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setActivePMeasModeComponent:
            m_pLambdaModulConfigData->m_activeMeasModeComponent = m_pXMLReader->getValue(key);
            break;
        case setActivePMeasModePhaseSelectComponent:
            m_pLambdaModulConfigData->m_activeMeasModePhaseComponent = m_pXMLReader->getValue(key);
            break;
        case setSystemCount:
        {
            lambdasystemconfiguration lsc;

            lsc.m_nInputPEntity = 1006; // some default
            lsc.m_sInputP = "ACT_PQS1";
            lsc.m_nInputQEntity = 1006;
            lsc.m_sInputQ = "ACT_PQS1";
            lsc.m_nInputSEntity = 1006;
            lsc.m_sInputS = "ACT_PQS1";

            m_pLambdaModulConfigData->m_nLambdaSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pLambdaModulConfigData->m_nLambdaSystemCount; i++)
            {
                m_ConfigXMLMap[QString("lambdamodconfpar:configuration:measure:system:lambda%1:p:inputentity").arg(i+1)] = setLambdaInputPEntity1 + i;
                m_ConfigXMLMap[QString("lambdamodconfpar:configuration:measure:system:lambda%1:p:component").arg(i+1)] = setLambdaInputPComponent1 + i;
                m_ConfigXMLMap[QString("lambdamodconfpar:configuration:measure:system:lambda%1:q:inputentity").arg(i+1)] = setLambdaInputQEntity1 + i;
                m_ConfigXMLMap[QString("lambdamodconfpar:configuration:measure:system:lambda%1:q:component").arg(i+1)] = setLambdaInputQComponent1 + i;
                m_ConfigXMLMap[QString("lambdamodconfpar:configuration:measure:system:lambda%1:s:inputentity").arg(i+1)] = setLambdaInputSEntity1 + i;
                m_ConfigXMLMap[QString("lambdamodconfpar:configuration:measure:system:lambda%1:s:component").arg(i+1)] = setLambdaInputSComponent1 + i;

                m_pLambdaModulConfigData->m_lambdaSystemConfigList.append(lsc);
                m_pLambdaModulConfigData->m_lambdaChannelList.append(QString("ACT_Lambda%1").arg(i+1));
            }
            break;
        }
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setLambdaInputPEntity1) && (cmd < setLambdaInputPEntity1 + 12))
            {
                cmd -= setLambdaInputPEntity1;
                // it is command for setting measuring mode

                lambdasystemconfiguration lsc = m_pLambdaModulConfigData->m_lambdaSystemConfigList.at(cmd);
                lsc.m_nInputPEntity = m_pXMLReader->getValue(key).toInt();
                m_pLambdaModulConfigData->m_lambdaSystemConfigList.replace(cmd, lsc);
            }
            else
                if ((cmd >= setLambdaInputQEntity1) && (cmd < setLambdaInputQEntity1 + 12))
                {
                    cmd -= setLambdaInputQEntity1;
                    // it is command for setting measuring mode

                    lambdasystemconfiguration lsc = m_pLambdaModulConfigData->m_lambdaSystemConfigList.at(cmd);
                    lsc.m_nInputQEntity = m_pXMLReader->getValue(key).toInt();
                    m_pLambdaModulConfigData->m_lambdaSystemConfigList.replace(cmd, lsc);
                }
            else
                if ((cmd >= setLambdaInputSEntity1) && (cmd < setLambdaInputSEntity1 + 12))
                {
                    cmd -= setLambdaInputSEntity1;
                    // it is command for setting measuring mode

                    lambdasystemconfiguration lsc = m_pLambdaModulConfigData->m_lambdaSystemConfigList.at(cmd);
                    lsc.m_nInputSEntity = m_pXMLReader->getValue(key).toInt();
                    m_pLambdaModulConfigData->m_lambdaSystemConfigList.replace(cmd, lsc);
                }
                else
                    if ((cmd >= setLambdaInputPComponent1) && (cmd < setLambdaInputPComponent1 + 12))
                    {
                        cmd -= setLambdaInputPComponent1;
                        // it is command for setting measuring mode

                        lambdasystemconfiguration lsc = m_pLambdaModulConfigData->m_lambdaSystemConfigList.at(cmd);
                        lsc.m_sInputP = m_pXMLReader->getValue(key);
                        m_pLambdaModulConfigData->m_lambdaSystemConfigList.replace(cmd, lsc);
                    }
                    else
                        if ((cmd >= setLambdaInputQComponent1) && (cmd < setLambdaInputQComponent1 + 12))
                        {
                            cmd -= setLambdaInputQComponent1;
                            // it is command for setting measuring mode

                            lambdasystemconfiguration lsc = m_pLambdaModulConfigData->m_lambdaSystemConfigList.at(cmd);
                            lsc.m_sInputQ = m_pXMLReader->getValue(key);
                            m_pLambdaModulConfigData->m_lambdaSystemConfigList.replace(cmd, lsc);
                        }
                        else
                            if ((cmd >= setLambdaInputSComponent1) && (cmd < setLambdaInputSComponent1 + 12))
                            {
                                cmd -= setLambdaInputSComponent1;
                                // it is command for setting measuring mode

                                lambdasystemconfiguration lsc = m_pLambdaModulConfigData->m_lambdaSystemConfigList.at(cmd);
                                lsc.m_sInputS = m_pXMLReader->getValue(key);
                                m_pLambdaModulConfigData->m_lambdaSystemConfigList.replace(cmd, lsc);
                            }

        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cLambdaModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
}

}

