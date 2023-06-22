#ifndef TEST_LOGGERCONTENTSESSIONLOADER_H
#define TEST_LOGGERCONTENTSESSIONLOADER_H

#include <QObject>

class test_loggercontentsessionloader : public QObject
{
    Q_OBJECT
private slots:
    void noSessionSetEmptyAvailableContentSets();

    void testSimpleAvailContentSets();
    void testSimpleEntityComponentsNoMatch();
    void testSimpleEntityComponentsAll();
};

#endif // TEST_LOGGERCONTENTSESSIONLOADER_H
