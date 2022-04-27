#ifndef XMLCONFIGREADER_PRIVATE_H
#define XMLCONFIGREADER_PRIVATE_H

#include "messagehandler.h"

#include <QList>
#include <QString>
#include <QHash>


/**
 * @b A 2d structure that mirrors some of the Qt container API functions and preserves the order of inserted values
 */
template<class T_key, class T_value> class FCKDUPHash {

public:
    void clear()
    {
        m_keyList.clear();
        m_keyValueHash.clear();
    }
    void insert(T_key key, T_value value)
    {
        if(!contains(key)) {
            m_keyList.append(key);
        }
        m_keyValueHash[key] = value;
    }
    T_value value(T_key key) const
    {
        T_value retVal;
        auto iter = m_keyValueHash.find(key);
        if(iter != m_keyValueHash.end()) {
            retVal = iter.value();
        }
        return retVal;
    }
    bool contains(T_key key) const
    {
        return m_keyList.contains(key);
    }
    QList<T_key> sortedKeys() const
    {
        return m_keyList;
    }
private:
    QList<T_key> m_keyList;
    QHash<T_key, T_value> m_keyValueHash;
};


namespace Zera
{
    namespace XMLConfig
    {
        class cReader;
        class cReaderPrivate
        {
        public:
            cReaderPrivate(cReader *parent);
        private:
            FCKDUPHash<QString, QString>data;
            QString schemaFilePath;
            MessageHandler messageHandler;
            Zera::XMLConfig::cReader *q_ptr;
            Q_DECLARE_PUBLIC(cReader)
        };
    }
}
#endif // XMLCONFIGREADER_PRIVATE_H
