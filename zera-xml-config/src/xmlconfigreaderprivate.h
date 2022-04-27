#ifndef XMLCONFIGREADER_PRIVATE_H
#define XMLCONFIGREADER_PRIVATE_H

#include "messagehandler.h"

#include <QList>
#include <QString>
#include <QDebug>


/**
 * @b A 2d structure that mirrors some of the Qt container API functions and preserves the order of inserted values
 */
template<class T_key, class T_value> class FCKDUPHash {

public:
    void clear()
    {
        m_keyList.clear();
        m_valueList.clear();
    }

    void insert(T_key key, T_value value)
    {
        if(contains(key) == false)
        {
            m_keyList.append(key);
            m_valueList.append(value);
        }
        else
        {
            m_valueList.replace(m_keyList.indexOf(key), value);
        }
    }


    T_value value(T_key key) const
    {
        T_value retVal;
        if(contains(key))
        {
            retVal = m_valueList.at(m_keyList.indexOf(key));
        }
        return retVal;
    }

    bool contains(T_key key) const
    {
        return m_keyList.contains(key);
    }

    QList<T_key> keys() const
    {
        return m_keyList;
    }

    QList<T_value> values() const
    {
        return m_valueList;
    }

private:

    QList<T_key> m_keyList;
    QList<T_value> m_valueList;
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
