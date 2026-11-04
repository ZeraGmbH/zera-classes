#ifndef TEST_CRO_CHANNEL_CAN_CHANGE_RANGE_OBSERVER_H
#define TEST_CRO_CHANNEL_CAN_CHANGE_RANGE_OBSERVER_H

#include <QObject>
#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>
#include <testserverforsenseinterfacemtxxxs2.h>

class test_cro_channel_can_change_range_observer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void cannotSetRangesAfterInit();
    void canSetRangesChannelsFetched();
    void cannotSetRangesChannelsFetchedOneRange();
    void canSetRangesAfterChannelsFetched();
    void cannotSetRangesAfterChannelsFetched();

    void cannotChangeInitialRangeAutomaticIsOn();
    void cannotChangeAfterRangeAutomaticIsOn();
    void canChangeAfterRangeAutomaticIsOnOff();

    void canChangeInitialSourceModeIsOff();
    void cannotChangeInitialSourceModeIsOn();
    void cannotChangeAfterSourceModeIsOn();
    void canChangeAfterSourceModeIsOnOff();

private:
    void setupServers();

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMtxxxs2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
};

#endif // TEST_CRO_CHANNEL_CAN_CHANGE_RANGE_OBSERVER_H
