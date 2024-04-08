#ifndef TEST_MODMAN_WITH_VF_LOGGER_H
#define TEST_MODMAN_WITH_VF_LOGGER_H

#include <QObject>

class test_modman_with_vf_logger : public QObject
{
    Q_OBJECT
private slots:
    void basicCheckRmsModule();
    void setupSytemWithLogger();
};

#endif // TEST_MODMAN_WITH_VF_LOGGER_H
