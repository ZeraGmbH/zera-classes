#include "xmlconfigreader.h"
#include "xmlconfigreaderprivate.h"

#include <QXmlStreamReader>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QtXmlPatterns/QXmlSchema>
#include <QStringList>
#include <QByteArray>
#include <QFile>
#include <QBuffer>

#include <QDebug>

namespace Zera
{
  namespace XMLConfig
  {
    cReader::cReader(QObject *parent) :
      QObject(parent), d_ptr(new cReaderPrivate())
    {
    }

    bool cReader::loadSchema(const QString &filePath)
    {
      bool retVal = false;
      QFile schemaFile(filePath);

      if(schemaFile.exists())
      {
        Q_D(cReader);
        QXmlSchema tmpSchema;
        if(tmpSchema.load(&schemaFile,QUrl(d->schemaFilePath)))
        {
          /// @todo evaluate wether clearing the data is reasonable
          d->data.clear();
          d->schemaFilePath=filePath;
          retVal = true;
        }
      }
      return retVal;
    }

    bool cReader::loadXML(const QString &filePath)
    {
      Q_D(cReader);
      bool retVal = false;
      QXmlSchema schema;
      QFile schemaFile(d->schemaFilePath);
      schemaFile.open(QFile::ReadOnly);

      if(schema.load(&schemaFile,QUrl(d->schemaFilePath)))
      {
        QXmlSchemaValidator sValidator(schema);

        //qDebug() << "schema is valid";

        if(sValidator.validate(QUrl(QString("file://%1").arg(filePath))))
        {
          QFile xmlFile(filePath);
          xmlFile.open(QFile::ReadOnly);

          //qDebug() << "XML is valid";
          if(d->xml2Config(&xmlFile))
          {
            retVal = true;
          }
        }
        else
        {
          qDebug() << "XML is invalid: " << filePath;
        }
      }
      else
      {
        qDebug() << "schema is invalid";
      }
      emit finishedParsingXML();
      return retVal;
    }

    bool cReader::loadXMLFromString(const QString &xmlString)
    {
      Q_D(cReader);
      bool retVal = false;
      QXmlSchema schema;
      QFile schemaFile(d->schemaFilePath);
      schemaFile.open(QFile::ReadOnly);

      if(schema.load(&schemaFile,QUrl(d->schemaFilePath)))
      {
        QXmlSchemaValidator sValidator(schema);

        //qDebug() << "schema is valid";

        if(sValidator.validate(xmlString))
        {
          QBuffer xmlDevice;
          xmlDevice.setData(xmlString.toUtf8());

          //qDebug() << "XML is valid";
          if(d->xml2Config(&xmlDevice))
          {
            retVal = true;
          }
        }
        else
        {
          qDebug() << "XML is invalid: " << xmlString;
        }
      }
      else
      {
        qDebug() << "schema is invalid";
      }
      emit finishedParsingXML();
      return retVal;
    }

    QVariant cReader::getValue(const QString &key)
    {
      Q_D(cReader);
      return d->data.value(key);
    }

    bool cReader::setValue(const QString &key, QVariant value)
    {
      Q_D(cReader);
      bool retVal=false;
      if(d->data.contains(key))
      {
        d->data.insert(key, value);
        retVal=true;
      }
      return retVal;
    }

    QString cReader::getXMLConfig()
    {
      Q_D(cReader);
      QString retVal = QString();
      QStringList parents, oldParents;
      QXmlStreamWriter stream(&retVal);
      stream.setAutoFormatting(true);
      stream.writeStartDocument();

      // for each key in the QHash
      for(int elemCount=0; elemCount<d->data.keys().count(); elemCount++)
      {
        QString elementName;
        parents = d->data.keys().at(elemCount).split(":");
        elementName = parents.last();
        parents.removeLast();

        if(!oldParents.isEmpty())
        {
          if(parents.count()>=oldParents.count())
          {
            //parse backwards
            for(int pCount=parents.count()-1; pCount>=0; pCount--)
            {
              if(pCount+1>oldParents.count())
              {
                continue;
              }
              else if(parents.at(pCount)!=oldParents.at(pCount))
              {
                stream.writeEndElement();
              }
            }
          }
          // in case we now have less parents than before
          else
          {
            for(int pCount=oldParents.count()-1; pCount>=0; pCount--)
            {
              if(pCount+1>parents.count())
              {
                stream.writeEndElement();
              }
            }

          }
        }
        // parse forward
        for(int pCount=0; pCount<parents.count(); pCount++)
        {
          // if we have more elements than before, write them
          if(pCount+1>oldParents.count())
          {
            stream.writeStartElement(parents.at(pCount));
          }
          // we don't have more elements than before but the element at this position is different
          else if(oldParents.at(pCount)!=parents.at(pCount))
          {
            stream.writeStartElement(parents.at(pCount));
          }
        }
        oldParents = parents;
        stream.writeTextElement(elementName, d->data.values().at(elemCount).toString());
      }
      stream.writeEndDocument();

      return retVal;
    }

  }
}
