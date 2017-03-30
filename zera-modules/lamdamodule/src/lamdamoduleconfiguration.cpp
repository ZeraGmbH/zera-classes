#include <QPoint>
#include <QString>
#include <xmlconfigreader.h>

#include "lamdamoduleconfiguration.h"
#include "lamdamoduleconfigdata.h"
#include "socket.h"

namespace LAMDAMODULE
{

cLamdaModuleConfiguration::cLamdaModuleConfiguration()
{
    m_pLamdaModulConfigData = 0;
    connect(m_pXMLReader, SIGNAL(valueChanged(const QString&)), this, SLOT(configXMLInfo(const QString&)));
    connect(m_pXMLReader, SIGNAL(finishedParsingXML(bool)), this, SLOT(completeConfiguration(bool)));
}


cLamdaModuleConfiguration::~cLamdaModuleConfiguration()
{
    if (m_pLamdaModulConfigData) delete m_pLamdaModulConfigData;
}


void cLamdaModuleConfiguration::setConfiguration(QByteArray xmlString)
{
    m_bConfigured = m_bConfigError = false;

    if (m_pLamdaModulConfigData) delete m_pLamdaModulConfigData;
    m_pLamdaModulConfigData = new cLamdaModuleConfigData();

    m_ConfigXMLMap.clear(); // in case of new configuration we completely set up

    // so now we can set up
    // initializing hash table for xml configuration

    m_ConfigXMLMap["lamdamodconfpar:configuration:connectivity:debuglevel"] = setDebugLevel;
    m_ConfigXMLMap["lamdamodconfpar:configuration:measure:system:n"] = setSystemCount;

    if (m_pXMLReader->loadSchema(defaultXSDFile))
        m_pXMLReader->loadXMLFromString(QString::fromUtf8(xmlString.data(), xmlString.size()));
    else
        m_bConfigError = true;
}


QByteArray cLamdaModuleConfiguration::exportConfiguration()
{
    return m_pXMLReader->getXMLConfig().toUtf8();
}


cLamdaModuleConfigData *cLamdaModuleConfiguration::getConfigurationData()
{
    return m_pLamdaModulConfigData;
}


void cLamdaModuleConfiguration::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        ok = true;
        int cmd = m_ConfigXMLMap[key];
        switch (cmd)
        {
        case setDebugLevel:
            m_pLamdaModulConfigData->m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSystemCount:
        {
            lamdasystemconfiguration lsc;

            lsc.m_nInputPEntity = 1006; // some default
            lsc.m_sInputP = "ACT_PQS1";
            lsc.m_nInputSEntity = 1006;
            lsc.m_sInputS = "ACT_PQS1";

            m_pLamdaModulConfigData->m_nLamdaSystemCount = m_pXMLReader->getValue(key).toInt(&ok);
            // here we generate dynamic hash entries for value channel configuration
            for (int i = 0; i < m_pLamdaModulConfigData->m_nLamdaSystemCount; i++)
            {
                m_ConfigXMLMap[QString("lamdamodconfpar:configuration:measure:system:lamda%1:p:inputentity").arg(i+1)] = setLamdaInputPEntity1 + i;
                m_ConfigXMLMap[QString("lamdamodconfpar:configuration:measure:system:lamda%1:p:component").arg(i+1)] = setLamdaInputPComponent1 + i;
                m_ConfigXMLMap[QString("lamdamodconfpar:configuration:measure:system:lamda%1:s:inputentity").arg(i+1)] = setLamdaInputSEntity1 + i;
                m_ConfigXMLMap[QString("lamdamodconfpar:configuration:measure:system:lamda%1:s:component").arg(i+1)] = setLamdaInputSComponent1 + i;

                m_pLamdaModulConfigData->m_lamdaSystemConfigList.append(lsc);
                m_pLamdaModulConfigData->m_lamdaChannelList.append(QString("ACT_LAMDA%1").arg(i+1));
            }
            break;
        }
        default:
            // here we decode the dyn. generated cmd's

            if ((cmd >= setLamdaInputPEntity1) && (cmd < setLamdaInputPEntity1 + 12))
            {
                cmd -= setLamdaInputPEntity1;
                // it is command for setting measuring mode

                lamdasystemconfiguration lsc = m_pLamdaModulConfigData->m_lamdaSystemConfigList.at(cmd);
                lsc.m_nInputPEntity = m_pXMLReader->getValue(key).toInt();
                m_pLamdaModulConfigData->m_lamdaSystemConfigList.replace(cmd, lsc);
            }
            else
                if ((cmd >= setLamdaInputSEntity1) && (cmd < setLamdaInputSEntity1 + 12))
                {
                    cmd -= setLamdaInputSEntity1;
                    // it is command for setting measuring mode

                    lamdasystemconfiguration lsc = m_pLamdaModulConfigData->m_lamdaSystemConfigList.at(cmd);
                    lsc.m_nInputSEntity = m_pXMLReader->getValue(key).toInt();
                    m_pLamdaModulConfigData->m_lamdaSystemConfigList.replace(cmd, lsc);
                }
                else
                    if ((cmd >= setLamdaInputPComponent1) && (cmd < setLamdaInputPComponent1 + 12))
                    {
                        cmd -= setLamdaInputPComponent1;
                        // it is command for setting measuring mode

                        lamdasystemconfiguration lsc = m_pLamdaModulConfigData->m_lamdaSystemConfigList.at(cmd);
                        lsc.m_sInputP = m_pXMLReader->getValue(key);
                        m_pLamdaModulConfigData->m_lamdaSystemConfigList.replace(cmd, lsc);
                    }
                    else
                        if ((cmd >= setLamdaInputSComponent1) && (cmd < setLamdaInputSComponent1 + 12))
                        {
                            cmd -= setLamdaInputSComponent1;
                            // it is command for setting measuring mode

                            lamdasystemconfiguration lsc = m_pLamdaModulConfigData->m_lamdaSystemConfigList.at(cmd);
                            lsc.m_sInputS = m_pXMLReader->getValue(key);
                            m_pLamdaModulConfigData->m_lamdaSystemConfigList.replace(cmd, lsc);
                        }

        }
        m_bConfigError |= !ok;
    }

    else
        m_bConfigError = true;
}


void cLamdaModuleConfiguration::completeConfiguration(bool ok)
{
    m_bConfigured = (ok && !m_bConfigError);
    emit configXMLDone();
}

}

