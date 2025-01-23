#ifndef TEST_SCPI_XSESSION_CHANGE_H
#define TEST_SCPI_XSESSION_CHANGE_H

#include <QObject>

class test_scpi_xsession_change : public QObject
{
    Q_OBJECT
private slots:
    void queryXSessionCatalog();
    void queryCurrentXSession();
    void setXSession();
};

#endif // TEST_SCPI_XSESSION_CHANGE_H
