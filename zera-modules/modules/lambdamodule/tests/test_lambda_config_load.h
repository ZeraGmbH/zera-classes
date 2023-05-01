#ifndef TEST_LAMBDA_CONFIG_LOAD_H
#define TEST_LAMBDA_CONFIG_LOAD_H

#include <QObject>

class test_lambda_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_LAMBDA_CONFIG_LOAD_H
