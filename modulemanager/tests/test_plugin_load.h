#ifndef TEST_PLUGIN_LOAD_H
#define TEST_PLUGIN_LOAD_H

#include <QObject>

class test_plugin_load : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void loadModulePluginsInstalled();
    void loadModulePluginsSources();
};

#endif // TEST_PLUGIN_LOAD_H
