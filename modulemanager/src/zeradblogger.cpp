#include "zeradblogger.h"
#include "diriteratorworker.h"

#include <vcmp_componentdata.h>
#include <vcmp_remoteproceduredata.h>
#include <vcmp_errordata.h>
#include <ve_commandevent.h>

#include <QStorageInfo>
#include <QtConcurrent>
#include <QDir>
#include <QDirIterator>
#include <QThread>

#include <functional>

class ZeraDBLoggerPrivate
{

    enum RPCResultCodes {
        RPC_CANCELED = -64,
        RPC_EINVAL = -EINVAL, //invalid parameters
        RPC_SUCCESS = 0
    };

    ZeraDBLoggerPrivate(ZeraDBLogger *t_qPtr) :
        m_qPtr(t_qPtr)
    {

    }

    ZeraDBLogger *m_qPtr=nullptr;
    //need an instance to call VeinLogger::AbstractLoggerDB::isValidDatabase
    VeinLogger::AbstractLoggerDB *m_validationDB;

    friend class ZeraDBLogger;
};

ZeraDBLogger::ZeraDBLogger(VeinLogger::DataSource *t_dataSource, VeinLogger::DBFactory t_factoryFunction, QObject *t_parent) :
    VeinLogger::DatabaseLogger(t_dataSource, t_factoryFunction, t_parent),
    m_dPtr(new ZeraDBLoggerPrivate(this))
{
    //create a database for validation
    m_dPtr->m_validationDB = t_factoryFunction(); //this class takes ownership
}

ZeraDBLogger::~ZeraDBLogger()
{
    delete m_dPtr->m_validationDB;
    delete m_dPtr;
}

bool ZeraDBLogger::processEvent(QEvent *t_event)
{
    return  VeinLogger::DatabaseLogger::processEvent(t_event);
}

