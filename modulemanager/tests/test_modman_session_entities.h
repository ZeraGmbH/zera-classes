#ifndef TEST_MODMAN_SESSION_ENTITITIES_H
#define TEST_MODMAN_SESSION_ENTITITIES_H

#include <QObject>

class test_modman_session_entities : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void loadAllSessionsAndOutputRegressionTestCodeCom5003();
    void loadAllSessionsAndOutputRegressionTestCodeMt310s2();

    void regressionCom5003Ced();
    void regressionCom5003Meas();
    void regressionCom5003PerPhase();
    void regressionCom5003Ref();

    void regressionMt310s2Ced();
    void regressionMt310s2Dc();
    void regressionMt310s2EmobAc();
    void regressionMt310s2EmobDc();
    void regressionMt310s2EmobAcDc();
    void regressionMt310s2Meas();

private:
    int generateCodeLists(QString device);
};

#endif // TEST_MODMAN_SESSION_ENTITITIES_H
