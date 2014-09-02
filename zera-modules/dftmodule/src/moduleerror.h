#ifndef MODULEERROR_H
#define MODULEERROR_H


#include <QObject>
#include <QString>
#include <QVariant>

class VeinPeer;
class VeinEntity;

namespace DFTMODULE
{

class cModuleError: public QObject
{
    Q_OBJECT
public:
    cModuleError(VeinPeer* peer,QString name);
    ~cModuleError(){}

public slots:
    void appendMsg(QString msg);

private:
    VeinPeer* m_pPeer;
    VeinEntity *m_pParEntity;
    QString m_sMessage;

private slots:
    void update(QVariant);
};

}

#endif // MODULEERROR_H
