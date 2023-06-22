#include "diriteratorworker.h"

#include <QCoreApplication>
#include <QThread>

DirIteratorWorker::DirIteratorWorker(const QString &t_searchPath, const QStringList &t_searchPatterns, QDir::Filter t_filter, QDirIterator::IteratorFlags t_flags, QObject *t_parent) :
    QObject(t_parent),
    m_iterator(t_searchPath, t_searchPatterns, t_filter, t_flags)
{

}

void DirIteratorWorker::setFilterFunction(DirIteratorWorker::FilterFunction t_function)
{
    if(t_function != nullptr)
    {
        m_filterFunction = t_function;
    }
}

void DirIteratorWorker::startSearch()
{
    QMutexLocker mutexLocker(&m_searchMutex);
    QString tmpPath;

    while(m_iterator.hasNext())
    {
        if(Q_UNLIKELY(QThread::currentThread()->isInterruptionRequested()))
        {
            emit sigInterrupted();
            return; //abort search
        }
        tmpPath = m_iterator.next();
        if(m_filterFunction(tmpPath) == true)
        {
            m_resultList.append(tmpPath);
            emit sigPartialResultReady(tmpPath);
        }
    }

    emit sigCompleteResultReady(m_resultList);
    emit sigFinished();
    return;
}
