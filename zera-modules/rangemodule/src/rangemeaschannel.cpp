#include <QRegExp>
#include <rminterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>

#include "debug.h"
#include "errormessages.h"
#include "rangemeaschannel.h"

namespace RANGEMODULE
{

cRangeMeasChannel::cRangeMeasChannel(Zera::Proxy::cProxy* proxy, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr, bool extend)
    :cBaseMeasChannel(proxy, rmsocket, pcbsocket, name, chnnr), m_bExtend(extend)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" meas channel
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceState);
    m_claimResourceState.addTransition(this, SIGNAL(activationContinue()), &m_pcbConnectionState);
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readChnAliasState);
    m_readChnAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, SIGNAL(activationContinue()), &m_readUnitState);
    m_readUnitState.addTransition(this, SIGNAL(activationContinue()), &m_readRangeAndProperties);
    m_readRangeAndProperties.addTransition(&m_rangeQueryMachine, SIGNAL(finished()), &m_resetStatusState);
    m_resetStatusState.addTransition(this, SIGNAL(activationContinue()), &m_setNotifierRangeCat);
    m_setNotifierRangeCat.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_claimResourceState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readDspChannelState);
    m_activationMachine.addState(&m_readChnAliasState);
    m_activationMachine.addState(&m_readSampleRateState);
    m_activationMachine.addState(&m_readUnitState);
    m_activationMachine.addState(&m_readRangeAndProperties);
    m_activationMachine.addState(&m_resetStatusState);
    m_activationMachine.addState(&m_setNotifierRangeCat);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_rmConnectState);

    connect(&m_rmConnectState, SIGNAL(entered()), SLOT(rmConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_claimResourceState, SIGNAL(entered()), SLOT(claimResource()));
    connect(&m_pcbConnectionState, SIGNAL(entered()), SLOT(pcbConnection()));
    connect(&m_readDspChannelState, SIGNAL(entered()), SLOT(readDspChannel()));
    connect(&m_readChnAliasState, SIGNAL(entered()), SLOT(readChnAlias()));
    connect(&m_readSampleRateState, SIGNAL(entered()), SLOT(readSampleRate()));
    connect(&m_readUnitState, SIGNAL(entered()), SLOT(readUnit()));
    connect(&m_readRangeAndProperties, SIGNAL(entered()), SLOT(readRangeAndProperties()));
    connect(&m_resetStatusState, SIGNAL(entered()), SLOT(resetStatusSlot()));
    connect(&m_setNotifierRangeCat, SIGNAL(entered()), SLOT(setNotifierRangeCat()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    // setting up statemachine for "deactivating" meas channel
    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationResetNotifiersState);
    m_deactivationResetNotifiersState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationResetNotifiersState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationResetNotifiersState, SIGNAL(entered()), SLOT(deactivationResetNotifiers()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

    // setting up statemachine for querying the meas channels ranges and their properties
    m_readRangelistState.addTransition(this, SIGNAL(activationContinue()), &m_readRngAliasState);
    m_readRngAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readTypeState);
    m_readTypeState.addTransition(this, SIGNAL(activationContinue()), &m_readUrvalueState);
    m_readUrvalueState.addTransition(this, SIGNAL(activationContinue()), &m_readRejectionState);
    m_readRejectionState.addTransition(this, SIGNAL(activationContinue()), &m_readOVRejectionState);
    m_readOVRejectionState.addTransition(this, SIGNAL(activationContinue()), &m_readisAvailState);
    m_readisAvailState.addTransition(this, SIGNAL(activationContinue()), &m_rangeQueryLoopState);
    m_rangeQueryLoopState.addTransition(this, SIGNAL(activationContinue()), &m_rangeQueryDoneState);
    m_rangeQueryLoopState.addTransition(this, SIGNAL(activationLoop()), &m_readRngAliasState);

    m_rangeQueryMachine.addState(&m_readRangelistState);
    m_rangeQueryMachine.addState(&m_readRngAliasState);
    m_rangeQueryMachine.addState(&m_readTypeState);
    m_rangeQueryMachine.addState(&m_readUrvalueState);
    m_rangeQueryMachine.addState(&m_readRejectionState);
    m_rangeQueryMachine.addState(&m_readOVRejectionState);
    m_rangeQueryMachine.addState(&m_readisAvailState);
    m_rangeQueryMachine.addState(&m_rangeQueryLoopState);
    m_rangeQueryMachine.addState(&m_rangeQueryDoneState);

    m_rangeQueryMachine.setInitialState(&m_readRangelistState);

    connect(&m_readRangelistState, SIGNAL(entered()), SLOT(readRangelist()));
    connect(&m_readRngAliasState, SIGNAL(entered()), SLOT(readRngAlias()));
    connect(&m_readTypeState, SIGNAL(entered()), SLOT(readType()));
    connect(&m_readUrvalueState, SIGNAL(entered()), SLOT(readUrvalue()));
    connect(&m_readRejectionState, SIGNAL(entered()), SLOT(readRejection()));
    connect(&m_readOVRejectionState, SIGNAL(entered()), SLOT(readOVRejection()));
    connect(&m_readisAvailState, SIGNAL(entered()), SLOT(readisAvail()));
    connect(&m_rangeQueryLoopState, SIGNAL(entered()), SLOT(rangeQueryLoop()));

    connect(&m_rangeQueryMachine, SIGNAL(finished()), this, SIGNAL(newRangeList()));
}


cRangeMeasChannel::~cRangeMeasChannel()
{
    delete m_pRMInterface;
    delete m_pPCBInterface;
}


quint32 cRangeMeasChannel::setRange(QString range)
{
    m_sNewRange = range;
    m_sActRange = m_sNewRange;
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->setRange(m_sName, m_RangeInfoHash[range].name);
        m_MsgNrCmdList[msgnr] = setmeaschannelrange;
        return msgnr;
    }
    else
        return 1;
}


quint32 cRangeMeasChannel::readGainCorrection(double amplitude)
{
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->getGainCorrection(m_sName, m_sActRange, amplitude);
        m_MsgNrCmdList[msgnr] = readgaincorrection;
        return msgnr;
    }
    else
        return 1;
}


quint32 cRangeMeasChannel::readOffsetCorrection(double amplitude)
{
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->getOffsetCorrection(m_sName, m_sActRange, amplitude);
        m_MsgNrCmdList[msgnr] = readoffsetcorrection;
        return msgnr;
    }
    else
        return 1;
}


quint32 cRangeMeasChannel::readStatus()
{
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->getStatus(m_sName);
        m_MsgNrCmdList[msgnr] = readmeaschannelstatus;
        return msgnr;
    }
    else
        return 1;
}


quint32 cRangeMeasChannel::resetStatus()
{
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->resetStatus(m_sName);
        m_MsgNrCmdList[msgnr] = resetmeaschannelstatus;
        return msgnr;
    }
    else
        return 1;

}


quint32 cRangeMeasChannel::readPhaseCorrection(double frequency)
{
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->getPhaseCorrection(m_sName, m_sActRange, frequency);
        m_MsgNrCmdList[msgnr] = readphasecorrection;
        return msgnr;
    }
    else
        return 1;
}


bool cRangeMeasChannel::isPossibleRange(QString range, double ampl)
{
    cRangeInfo& ri = m_RangeInfoHash[range];
    return ((ri.urvalue * ri.ovrejection *sqrt2 / ri.rejection) >= ampl);
    // return (m_RangeInfoHash[range].urvalue * sqrt2 >= ampl);
}


bool cRangeMeasChannel::isPossibleRange(QString range)
{
    return m_RangeInfoHash.contains(range);
}


bool cRangeMeasChannel::isOverload(double ampl)
{
    cRangeInfo& ri = m_RangeInfoHash[m_sActRange];
    // qDebug() << QString("Ampl: %1; Rng: %2").arg(ampl).arg(ri.alias);
    return ((ri.urvalue * ri.ovrejection * sqrt2 /ri.rejection) < ampl);
}


QString cRangeMeasChannel::getOptRange(double ampl)
{
    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = 1e32;
    double newUrvalue;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++)
    {
        const cRangeInfo& ri = riList.at(i);
        newUrvalue = ri.urvalue;
        if (((newUrvalue * sqrt2 *ri.ovrejection /ri.rejection ) >= ampl) && (newUrvalue < newAmpl))
        {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    if (p > -1)
        return riList.at(p).alias;
    else
        return getMaxRange(); // we return maximum range in case of overload condtion
}


QString cRangeMeasChannel::getMaxRange()
{
    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = 0;
    double newUrvalue;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++)
    {
        newUrvalue = riList.at(i).urvalue;
        if (newUrvalue > newAmpl)
        {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    return riList.at(p).alias;
}


QString cRangeMeasChannel::getRangeListAlias()
{
    return m_sRangeListAlias;
}


double cRangeMeasChannel::getGainCorrection()
{
    return m_fGainCorrection;
}


double cRangeMeasChannel::getPhaseCorrection()
{
    return m_fPhaseCorrection;
}


double cRangeMeasChannel::getOffsetCorrection()
{
    return m_fOffsetCorrection;
}


bool cRangeMeasChannel::isHWOverload()
{
    return ((m_nStatus & 1) > 0);
}


double cRangeMeasChannel::getUrValue(QString range)
{
    return m_RangeInfoHash[range].urvalue;
}


double cRangeMeasChannel::getUrValue()
{
    return m_RangeInfoHash[m_sActRange].urvalue;
}


double cRangeMeasChannel::getRejection(QString range)
{
    return m_RangeInfoHash[range].rejection;
}


double cRangeMeasChannel::getRejection()
{
    return m_RangeInfoHash[m_sActRange].rejection;
}


double cRangeMeasChannel::getOVRRejection(QString range)
{
    return m_RangeInfoHash[range].ovrejection;
}


double cRangeMeasChannel::getOVRRejection()
{
    return m_RangeInfoHash[m_sActRange].ovrejection;
}


double cRangeMeasChannel::getMaxRangeUrvalueMax()
{
   QString s = getMaxRange();
   return (getUrValue(s) * getOVRRejection(s) / getRejection(s));
}


double cRangeMeasChannel::getRangeUrvalueMax()
{
    return (getUrValue() * getOVRRejection() / getRejection());
}


void cRangeMeasChannel::generateInterface()
{
}


void cRangeMeasChannel::deleteInterface()
{
}


void cRangeMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        // qDebug() << "meas program interrupt";
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case 1:
            // we got a sense:chn:range:cat notifier
            // so we have to read the new range list and properties
            if (!m_rangeQueryMachine.isRunning())
                m_rangeQueryMachine.start();
            break;
        }
    }

    else
    {
        int cmd = m_MsgNrCmdList.take(msgnr);

        switch (cmd)
        {
        case sendmeaschannelrmident:
            if (reply == ack) // we only continue if resource manager acknowledges
                emit activationContinue();
            else
            {
                emit errMsg(tr(rmidentErrMSG));
    #ifdef DEBUG
                qDebug() << rmidentErrMSG;
    #endif
                emit activationError();
            }
            break;

        case readresourcetypes:
            if ((reply == ack) && (answer.toString().contains("SENSE")))
                emit activationContinue();
            else
            {
                emit errMsg((tr(resourcetypeErrMsg)));
    #ifdef DEBUG
                qDebug() << resourcetypeErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readresource:
            if ((reply == ack) && (answer.toString().contains(m_sName)))
                emit activationContinue();
            else
            {
                emit errMsg((tr(resourceErrMsg)));
    #ifdef DEBUG
                qDebug() << resourceErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readresourceinfo:
        {
            bool ok1, ok2, ok3;
            int max, free;
            QStringList sl;


            sl = answer.toString().split(';');
            if ((reply ==ack) && (sl.length() >= 4))
            {
                max = sl.at(0).toInt(&ok1); // fixed position
                free = sl.at(1).toInt(&ok2);
                m_sDescription = sl.at(2);
                m_nPort = sl.at(3).toInt(&ok3);

                if (ok1 && ok2 && ok3 && ((max == free) == 1))
                {
                    emit activationContinue();
                }

                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
    #ifdef DEBUG
                    qDebug() << resourceInfoErrMsg;
    #endif
                    emit activationError();
                }
            }

            else
            {
                emit errMsg((tr(resourceInfoErrMsg)));
    #ifdef DEBUG
                qDebug() << resourceInfoErrMsg;
    #endif
                emit activationError();
            }

            break;

        }
        case claimresource:
            if (reply == ack)
                emit activationContinue();
            else
            {
                emit errMsg((tr(claimresourceErrMsg)));
    #ifdef DEBUG
                qDebug() << claimresourceErrMsg;
    #endif
                emit activationError();
            }
            break;

        case freeresource:
            if (reply == ack || reply == nack) // we accept nack here also
                emit deactivationContinue(); // maybe that resource was deleted by server and then it is no more set
            else
            {
                emit errMsg((tr(freeresourceErrMsg)));
    #ifdef DEBUG
                qDebug() << freeresourceErrMsg;
    #endif
                emit deactivationError();
            }
            break;

        case unregisterNotifiers:
            if (reply == ack)
                emit deactivationContinue();
            {
                emit errMsg((tr(unregisterpcbnotifierErrMsg)));
    #ifdef DEBUG
                qDebug() << unregisterpcbnotifierErrMsg;
    #endif
                emit deactivationError();
            }
            break;

        case readdspchannel:
            if (reply == ack)
            {
                m_nDspChannel = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readdspchannelErrMsg)));
    #ifdef DEBUG
                qDebug() << readdspchannelErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readchnalias:
            if (reply == ack)
            {
                m_sAlias = answer.toString();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readaliasErrMsg)));
    #ifdef DEBUG
                qDebug() << readaliasErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readsamplerate:
            if (reply == ack)
            {
                m_nSampleRate = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readsamplerateErrMsg)));
    #ifdef DEBUG
                qDebug() << readsamplerateErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readunit:
            if (reply == ack)
            {
                m_sUnit = answer.toString();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readunitErrMsg)));
    #ifdef DEBUG
                qDebug() << readunitErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readrangelist:
            if (reply == ack)
            {
                m_RangeNameList = answer.toStringList();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangelistErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangelistErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readrngalias:
            if (reply == ack)
            {
                ri.alias = answer.toString();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangealiasErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangealiasErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readtype:
            if (reply == ack)
            {
                ri.type = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangetypeErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangetypeErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readurvalue:
            if (reply == ack)
            {
                ri.urvalue = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangeurvalueErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangeurvalueErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readrejection:
            if (reply == ack)
            {
                ri.rejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangerejectionErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangerejectionErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readovrejection:
            if (reply == ack)
            {
                ri.ovrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangeovrejectionErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangeovrejectionErrMsg;
    #endif
                emit activationError();
            }
            break;

        case readisavail:
            if (reply == ack)
            {
                ri.avail = answer.toBool();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangeavailErrMsg)));
    #ifdef DEBUG
                qDebug() << readrangeavailErrMsg;
    #endif
                emit activationError();
            }
            break;

        case setmeaschannelrange:
            if (reply == ack)
                m_sActRange = m_sNewRange;
            else
            {
                emit errMsg((tr(setRangeErrMsg)));
    #ifdef DEBUG
                qDebug() << setRangeErrMsg;
    #endif
                emit executionError();
            }; // perhaps some error output
            emit cmdDone(msgnr);
            break;

        case readgaincorrection:
            if (reply == ack)
                m_fGainCorrection = answer.toDouble(&ok);
            else
            {
                emit errMsg((tr(readGainCorrErrMsg)));
    #ifdef DEBUG
                qDebug() << readGainCorrErrMsg;
    #endif
            };
            emit cmdDone(msgnr);
            break;

        case readoffsetcorrection:
            if (reply == ack)
                m_fOffsetCorrection = answer.toDouble(&ok);
            else
            {
                emit errMsg((tr(readOffsetCorrErrMsg)));
    #ifdef DEBUG
                qDebug() << readOffsetCorrErrMsg;
    #endif
                emit executionError();
            };
            emit cmdDone(msgnr);
            break;

        case readphasecorrection:
            if (reply == ack)
                m_fPhaseCorrection = answer.toDouble(&ok);
            else
            {
                emit errMsg((tr(readPhaseCorrErrMsg)));
    #ifdef DEBUG
                qDebug() << readPhaseCorrErrMsg;
    #endif
                emit executionError();
            };
            emit cmdDone(msgnr);
            break;

        case readmeaschannelstatus:
            if (reply == ack)
                m_nStatus = answer.toInt(&ok);
            else
            {
                emit errMsg((tr(readChannelStatusErrMsg)));
    #ifdef DEBUG
                qDebug() << readChannelStatusErrMsg;
    #endif
                emit executionError();
            };
            emit cmdDone(msgnr);
            break;

        case resetmeaschannelstatus:
            if (reply == ack)
                {}
            else
            {
                emit errMsg((tr(resetChannelStatusErrMsg)));
    #ifdef DEBUG
                qDebug() << resetChannelStatusErrMsg;
    #endif
                emit executionError();
            }; // perhaps some error output
            emit cmdDone(msgnr);
            break;

        case resetmeaschannelstatus2:
            if (reply == ack)
            {
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(resetChannelStatusErrMsg)));
    #ifdef DEBUG
                qDebug() << resetChannelStatusErrMsg;
    #endif
                emit activationError();
            }; // perhaps some error output
            break;

        case registerNotifier:
            if (reply == ack)
            {
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(registerpcbnotifierErrMsg)));
    #ifdef DEBUG
                qDebug() << registerpcbnotifierErrMsg;
    #endif
                emit activationError();
            }; // perhaps some error output
            break;
        }
    }
}


void cRangeMeasChannel::setRangeListAlias()
{
    QString s;
    QList<cRangeInfo> riList = m_RangeInfoHash.values();

    // first we sort the range alias according to upper range values
    for (int i = 0; i < riList.count()-1; i++)
        for (int j = i; j < riList.count(); j++)
            if (riList.at(i).urvalue < riList.at(j).urvalue)
                riList.swap(i, j);

    // second we sort the range alias according to range type
    for (int i = 0; i < riList.count()-1; i++)
        for (int j = i; j < riList.count(); j++)
            if (riList.at(i).type < riList.at(j).type)
                riList.swap(i, j);

    s = riList.at(0).alias;
    for (int i = 1; i < riList.count(); i++)
        s = s + ";" + riList.at(i).alias;

    m_sRangeListAlias = s;
}


void cRangeMeasChannel::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_pRMSocket->m_sIP, m_pRMSocket->m_nPort);
    m_rmConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    // todo insert timer for timeout

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pRMClient);
    // resource manager liste sense abfragen
    // bin ich da drin ?
    // nein -> fehler activierung
    // ja -> socket von rm besorgen
    // resource bei rm belegen
    // beim pcb proxy server interface beantragen

    // quint8 m_nDspChannel; dsp kanal erfragen
    // QString m_sAlias; kanal alias erfragen
    // eine liste aller möglichen bereichen erfragen
    // d.h. (avail = 1 und type =1
    // und von diesen dann
    // alias, urvalue, rejection und ovrejection abfragen
}


void cRangeMeasChannel::sendRMIdent()
{
   m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("MeasChannel%1").arg(m_nChannelNr))] = sendmeaschannelrmident;
}


void cRangeMeasChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
}


void cRangeMeasChannel::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = readresource;
}


void cRangeMeasChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", m_sName)] = readresourceinfo;
}


void cRangeMeasChannel::claimResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SENSE", m_sName, 1)] = claimresource;
}


void cRangeMeasChannel::pcbConnection()
{
    m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_nPort);
    m_pcbConnectionState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_readDspChannelState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cRangeMeasChannel::readDspChannel()
{
   m_MsgNrCmdList[m_pPCBInterface->getDSPChannel(m_sName)] = readdspchannel;
}


void cRangeMeasChannel::readChnAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName)] = readchnalias;
}


void cRangeMeasChannel::readSampleRate()
{
    m_MsgNrCmdList[m_pPCBInterface->getSampleRate()] = readsamplerate;
}


void cRangeMeasChannel::readUnit()
{
    m_MsgNrCmdList[m_pPCBInterface->getUnit(m_sName)] = readunit;
}


void cRangeMeasChannel::readRangeAndProperties()
{
    m_rangeQueryMachine.start(); // read all ranges and their properties
}


void cRangeMeasChannel::resetStatusSlot()
{
    m_MsgNrCmdList[m_pPCBInterface->resetStatus(m_sName)] = resetmeaschannelstatus2;
}


void cRangeMeasChannel::setNotifierRangeCat()
{
    if (m_bExtend)
        m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(m_sName),"1")] = registerNotifier;
    else
        emit activationContinue();
}


void cRangeMeasChannel::activationDone()
{
    m_bActive = true;
    emit activated();
}


void cRangeMeasChannel::deactivationInit()
{
    // deactivation means we have to free our resources
    m_bActive = false;
    m_MsgNrCmdList[m_pRMInterface->freeResource("SENSE", m_sName)] = freeresource;
}


void cRangeMeasChannel::deactivationResetNotifiers()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisterNotifiers;
}


void cRangeMeasChannel::deactivationDone()
{
    m_pProxy->releaseConnection(m_pRMClient);
    m_pProxy->releaseConnection(m_pPCBClient);
    // and disconnect for our servers afterwards
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}


void cRangeMeasChannel::readRangelist()
{
    m_RangeInfoHash.clear();
    m_MsgNrCmdList[m_pPCBInterface->getRangeList(m_sName)] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}



void cRangeMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrngalias;
}


void cRangeMeasChannel::readType()
{
    m_MsgNrCmdList[m_pPCBInterface->getType(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readtype;
}


void cRangeMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pPCBInterface->getUrvalue(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readurvalue;
}


void cRangeMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pPCBInterface->getRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrejection;
}


void cRangeMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pPCBInterface->getOVRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readovrejection;
}


void cRangeMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pPCBInterface->isAvail(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
}


void cRangeMeasChannel::rangeQueryLoop()
{
    ri.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHash[ri.alias] = ri; // for each range we append cRangeinfo per alias

    m_RangeQueryIt++;
    if (m_RangeQueryIt < m_RangeNameList.count()) // another range ?
        emit activationLoop();
    else
    {
        QHash<QString, cRangeInfo>::iterator it = m_RangeInfoHash.begin();
        while (it != m_RangeInfoHash.end()) // we delete all unused ranges
        {
            ri = it.value();
            if (ri.avail)
                ++it;
            else
                it = m_RangeInfoHash.erase(it); // in case range is not avail
        }

        setRangeListAlias(); // and the list of possible ranges alias
        emit activationContinue();
    }
}


}
