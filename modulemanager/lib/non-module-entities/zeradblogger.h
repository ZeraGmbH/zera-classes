#ifndef ZERADBLOGGER_H
#define ZERADBLOGGER_H

#include <vl_databaselogger.h>

namespace VeinLogger
{
class DataSource;
}

class ZeraDBLoggerPrivate;

/**
 * @brief Class to avoid adding policy to the VeinLogger::DatabaseLogger
 */
class ZeraDBLogger : public VeinLogger::DatabaseLogger
{
    Q_OBJECT
public:
    ZeraDBLogger(VeinLogger::DataSource *t_dataSource, VeinLogger::DBFactory t_factoryFunction, QObject *parent=nullptr);
    ~ZeraDBLogger() override;
private:
    void processEvent(QEvent *t_event) override;
    ZeraDBLoggerPrivate *m_dPtr=nullptr;
    friend class ZeraDBLoggerPrivate;
};

#endif // ZERADBLOGGER_H
