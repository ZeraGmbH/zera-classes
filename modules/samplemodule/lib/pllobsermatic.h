#ifndef PLLOBSERMATIC_H
#define PLLOBSERMATIC_H

// cPllObsermatic is responsible for setting, observing, automatic setting
// of pll channels.

#include "samplemoduleconfigdata.h"
#include <moduleactivist.h>
#include <vfmoduleparameter.h>
#include <QFinalState>

namespace SAMPLEMODULE
{
enum sampleObsermaticCmds
{
    setpll
};

class cSampleModule;

/*class cPllObsermatic: public cModuleActivist
{
    Q_OBJECT
public:
    cPllObsermatic(cSampleModule* module, cSampleModuleConfigData& confData);
    virtual ~cPllObsermatic();
    void generateVeinInterface() override;
    QString getAliasFromSystemName(QString systemName);
public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);

private slots:
    void activationDone();

    void deactivationInit();
    void deactivationDone();

private:
    void setNewPLLChannel();

    cSampleModule *m_pModule;
    cSampleModuleConfigData& m_ConfPar;
    QString m_sNewPllChannelAlias;
    QVector<float> m_ActualValues; // here we find the actual values

    QHash<QString,QString> m_AliasHash; // systemname -> alias


    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activation
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;
};*/

}

#endif // PLLOBSERMATIC_H
