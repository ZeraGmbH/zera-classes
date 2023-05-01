#ifndef TEST_FFT_CONFIG_LOAD_H
#define TEST_FFT_CONFIG_LOAD_H

#include <QObject>

class test_fft_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
};

#endif // TEST_FFT_CONFIG_LOAD_H
