#include "testactualvaluegeneratorrms.h"

TestActualValueGeneratorRms::TestActualValueGeneratorRms(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList)
{
}

void TestActualValueGeneratorRms::onNewActualValues(QVector<float> *actValues)
{
    if(m_started) {
        m_actValues = *actValues;
        emit sigNewActualValues(&m_actValues);
    }
}

void TestActualValueGeneratorRms::start()
{
    m_started = true;
}

void TestActualValueGeneratorRms::stop()
{
    m_started = false;
}

QStringList TestActualValueGeneratorRms::getValueChannelList()
{
    return m_valueChannelList;
}
