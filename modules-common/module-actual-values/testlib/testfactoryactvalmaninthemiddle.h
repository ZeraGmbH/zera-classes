#ifndef TESTFACTORYACTVALMANINTHEMIDDLE_H
#define TESTFACTORYACTVALMANINTHEMIDDLE_H

#include "abstractfactoryactvalmaninthemiddle.h"
#include "testactvalmaninthemiddle.h"
#include <QHash>

class TestFactoryActValManInTheMiddle : public AbstractFactoryActValManInTheMiddle
{
public:
    AbstractActValManInTheMiddlePtr getActValGeneratorDft(int entityId, QStringList valueChannelList = QStringList()) override;
    TestActValManInTheMiddlePtr getActValGeneratorDftTest(int entityId);
private:
    QHash<int, TestActValManInTheMiddlePtr> m_ActValGenerators;
};

typedef std::shared_ptr<TestFactoryActValManInTheMiddle> TestFactoryActValManInTheMiddlePtr;

#endif // TESTFACTORYACTVALMANINTHEMIDDLE_H
