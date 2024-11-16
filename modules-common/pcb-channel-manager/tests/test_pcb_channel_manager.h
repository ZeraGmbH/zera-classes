#ifndef TEST_PCB_CHANNEL_MANAGER_H
#define TEST_PCB_CHANNEL_MANAGER_H

#include <QObject>

class test_pcb_channel_manager : public QObject
{
    Q_OBJECT
private slots:
    void emptyChannelListOnStartup();
};

#endif // TEST_PCB_CHANNEL_MANAGER_H
