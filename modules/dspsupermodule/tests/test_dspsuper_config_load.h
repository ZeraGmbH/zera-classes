#ifndef test_dspsuper_config_load_H
#define test_dspsuper_config_load_H

#include <QObject>

class test_dspsuper_config_load : public QObject
{
    Q_OBJECT
private slots:
    void fileFound();
    void allFilesLoaded();
    void writtenXmlIsStillValid();
};

#endif // test_dspsuper_config_load_H
