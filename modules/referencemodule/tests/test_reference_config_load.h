#ifndef TEST_FFT_CONFIG_LOAD_H
#define TEST_FFT_CONFIG_LOAD_H

#include <QObject>

class test_reference_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // TEST_FFT_CONFIG_LOAD_H
