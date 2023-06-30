#ifndef DDIRITERATORWORKER_H
#define DDIRITERATORWORKER_H

#include <QObject>
#include <QDirIterator>
#include <QMutex>
#include <QTimer>
#include <functional>

/**
 * @brief Amalgamation of features from QDirIterator, QtConcurrent::filtered and QFutureWatcher (AKA pure boilerplate code)
 * The QDirIterator is not a classical interator, maybe it could have been programmed as forward iterator but the current impelmentation is incompatible with QtConcurrent functions
 * This class was created in order to stream search results and provide cancellation functionality (QtConcurrent::run doesn't support this).
 */
class DirIteratorWorker : public QObject
{
    Q_OBJECT
public:
    using FilterFunction = std::function<bool (QString)>;

    explicit DirIteratorWorker(const QString &t_searchPath, const QStringList &t_searchPatterns, QDir::Filter t_filter, QDirIterator::IteratorFlags t_flags=QDirIterator::NoIteratorFlags, QObject *t_parent = nullptr);
    /**
   * @brief sets m_filterFunction
   * @param t_function a function matching the signature of FilterFunction
   */
    void setFilterFunction(FilterFunction t_function);

signals:
    /**
   * @brief Emits when the search is completed
   */
    void sigFinished();
    /**
   * @brief see http://doc.qt.io/qt-5/qthread.html#isInterruptionRequested
   */
    void sigInterrupted();
    /**
   * @brief Emits for each entry of the result list
   * @param t_partialResult
   */
    void sigPartialResultReady(QString t_partialResult);
    /**
   * @brief Emits when the search result is available
   * @param t_result
   */
    void sigCompleteResultReady(QStringList t_result);

public slots:
    void startSearch();

private:
    static bool defaultFilter(QString) {return true;}

    QDirIterator m_iterator;
    QStringList m_resultList;
    QMutex m_searchMutex;
    FilterFunction m_filterFunction = defaultFilter;
    bool m_running=false;
};

#endif // DDIRITERATORWORKER_H
