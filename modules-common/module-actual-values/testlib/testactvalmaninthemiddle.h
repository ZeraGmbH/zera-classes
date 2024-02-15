#ifndef TESTACTVALMANINTHEMIDDLE_H
#define TESTACTVALMANINTHEMIDDLE_H

#include "abstractactvalmaninthemiddle.h"

class TestActValManInTheMiddle : public AbstractActValManInTheMiddle
{
    Q_OBJECT
public:
    TestActValManInTheMiddle(QStringList valueChannelList);

    void onNewActualValues(QVector<float> *actValues) override;
    void start() override;
    void stop() override;

    QStringList getValueChannelList();
private:
    QStringList m_valueChannelList;
    QVector<float> m_actValues;
    bool m_started = false;
};

typedef std::shared_ptr<TestActValManInTheMiddle> TestActValManInTheMiddlePtr;

#endif // TESTACTVALMANINTHEMIDDLE_H
