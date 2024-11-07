#ifndef TEST_OSCI_CONFIG_LOAD_H
#define TEST_OSCI_CONFIG_LOAD_H

#include <QObject>

class test_osci_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_OSCI_CONFIG_LOAD_H
