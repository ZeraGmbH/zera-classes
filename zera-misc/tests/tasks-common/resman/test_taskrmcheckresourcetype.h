#ifndef TEST_TASKRMCHECKRESOURCETYPE_H
#define TEST_TASKRMCHECKRESOURCETYPE_H

#include <QObject>

class test_taskrmcheckresourcetype : public QObject
{
    Q_OBJECT
private slots:
    void okOnMatchingResourceLowerCase();
    void okOnMatchingResourceUpperCase();
    void errorOnNoResources();
    void errorOnMissingResource();
};

#endif // TEST_TASKRMCHECKRESOURCETYPE_H
