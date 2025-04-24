#ifndef TEST_API_CONFIG_LOAD_H
#define TEST_API_CONFIG_LOAD_H

#include <QObject>

class test_api_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_API_CONFIG_LOAD_H
