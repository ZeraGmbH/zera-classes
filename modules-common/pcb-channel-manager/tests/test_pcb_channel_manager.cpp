#include "test_pcb_channel_manager.h"
#include "pcbchannelmanager.h"
#include <QTest>
#include <QSignalSpy>

QTEST_MAIN(test_pcb_channel_manager)

void test_pcb_channel_manager::emptyChannelListOnStartup()
{
    PcbChannelManager manager;
    QVERIFY(manager.getChannelMNames().isEmpty());
}
