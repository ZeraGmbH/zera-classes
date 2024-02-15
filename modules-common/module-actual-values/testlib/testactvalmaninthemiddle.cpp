#include "testactvalmaninthemiddle.h"

TestActValManInTheMiddle::TestActValManInTheMiddle(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList)
{
}

void TestActValManInTheMiddle::onNewActualValues(QVector<float> *actValues)
{
    if(m_started) {
        m_actValues = *actValues;
        emit sigNewActualValues(&m_actValues);
    }
}

void TestActValManInTheMiddle::start()
{
    m_started = true;
}

void TestActValManInTheMiddle::stop()
{
    m_started = false;
}

QStringList TestActValManInTheMiddle::getValueChannelList()
{
    return m_valueChannelList;
}
