#ifndef ELECTRONICBURDENDEVICEVEIN_H
#define ELECTRONICBURDENDEVICEVEIN_H

#include <QObject>
#include "veininterface.h"

class electronicburdendevicevein : public QObject
{
    Q_OBJECT
public:
    explicit electronicburdendevicevein();

signals:
private:
    VeinInterface * m_pVeinInterface = nullptr;
};

#endif // ELECTRONICBURDENDEVICEVEIN_H
