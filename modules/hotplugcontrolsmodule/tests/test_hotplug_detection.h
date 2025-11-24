#ifndef TEST_HOTPLUG_DETECTION_H
#define TEST_HOTPLUG_DETECTION_H

#include <testjsonspyeventsystem.h>
#include <vf_core_stack_client.h>
#include <QObject>

class test_hotplug_detection : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void noHotplug();
    void oneEmobConnected();
    void oneMt650eConnected();
    void twoEmobsConnected();
    void twoMt650eConnected();
    void oneEmobOneMt650eConnected();

private:
};

#endif // TEST_HOTPLUG_DETECTION_H
