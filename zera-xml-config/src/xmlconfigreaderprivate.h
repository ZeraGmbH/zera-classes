#ifndef XMLCONFIGREADER_PRIVATE_H
#define XMLCONFIGREADER_PRIVATE_H

#include "messagehandler.h"
#include <QList>
#include <QString>
#include <QHash>

template<class T_key, class T_value> class HashWithSortedKeys
{
public:
    void clear()
    {
        m_keyList.clear();
        m_keyValueHash.clear();
    }
    void insert(T_key key, T_value value)
    {
        if(!m_keyList.contains(key)) {
            m_keyList.append(key);
        }
        m_keyValueHash[key] = value;
    }
    bool set(T_key key, T_value value)
    {
        auto iter = m_keyValueHash.find(key);
        if(iter != m_keyValueHash.end()) {
            iter.value() = value;
            return true;
        }
        return false;
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
    QList<T_key> getKeysSortedByCreationSequence() const
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
            HashWithSortedKeys<QString, QString>data;
            QString schemaFilePath;
            MessageHandler messageHandler;
            Zera::XMLConfig::cReader *q_ptr;
            Q_DECLARE_PUBLIC(cReader)
        };
    }
}
#endif // XMLCONFIGREADER_PRIVATE_H
