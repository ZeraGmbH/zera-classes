#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H
#include <QObject>
#include <QList>

QT_BEGIN_NAMESPACE

class QFinalState;
class QState;
class QStateMachine;

QT_END_NAMESPACE

class VeinPeer;
class cSCPIConnection;

namespace ZeraModules
{
  class VirtualModule : public QObject
  {
    Q_OBJECT
  public:
    explicit VirtualModule(QObject *qObjParent=0) : QObject(qObjParent) {}
    virtual ~VirtualModule() {}
    virtual QList<const QState*> getActualStates()=0; // in case parallel working states
    virtual void setConfiguration(QByteArray xmlConfigData)=0; // here we set configuration and parameters
    virtual QByteArray getConfiguration()=0;
    virtual bool isConfigured()=0;

    virtual void startModule()=0;
    virtual void stopModule()=0;

    virtual void setPeer(VeinPeer *vPeer)=0;
    virtual void setScpiControls(cSCPIConnection *scpi)=0;

    // public slots:
    //	void setSCPIConnection(cSCPIConnection* connection); // we receive a scpi connection

    //  signals:
    //    void moduleError(QString errorString);
    //    void getSCPIConnection(quint16 port); // here we request a scpi connection

  protected:
    QStateMachine *m_pStateMachine;

    QState *m_pStateIdle;
    QState *m_pStateConfigure;
    QState *m_pStateRun;
    QState *m_pStateStop;
    QFinalState *m_pStateFinished;
  };
}


#endif // MODULEOBJECT_H
