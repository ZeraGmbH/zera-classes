#ifndef TEST_SAMPLE_CONFIG_LOAD_H
#define TEST_SAMPLE_CONFIG_LOAD_H

#include <QObject>

class test_sample_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
    void noWritesOnModuleLoad();

    void startChannelUL1AutomaticOff();
    void startChannelUL2AutomaticOff();
    void startChannelFixedFreqAutomaticOff();

    void startChannelIL1AutomaticOn();
};

#endif // TEST_SAMPLE_CONFIG_LOAD_H
