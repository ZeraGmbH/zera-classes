#include "xmlconfigreader.h"
#include "xmlconfigreaderprivate.h"

#include <QXmlStreamReader>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QtXmlPatterns/QXmlSchema>
#include <QStringList>
#include <QByteArray>
#include <QFile>
#include <QBuffer>
#include <QIODevice>

#include <QDebug>

namespace Zera
{
  namespace XMLConfig
  {
    cReader::cReader(QObject *parent) :
      QObject(parent), d_ptr(new cReaderPrivate(this))
    {
    }

    bool cReader::loadSchema(QString filePath)
    {
      bool retVal = false;
      QFile schemaFile(filePath);

      if(schemaFile.exists())
      {
        Q_D(cReader);
        schemaFile.open(QFile::ReadOnly);
        QXmlSchema tmpSchema;
        if(tmpSchema.load(&schemaFile,QUrl(filePath)))
        {
          /// @todo evaluate wether clearing the data is reasonable
          d->data.clear();
          d->schemaFilePath=filePath;
          retVal = true;
        }
      }
      return retVal;
    }

    bool cReader::loadXML(QString filePath)
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
          if(xml2Config(&xmlFile))
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

    bool cReader::loadXMLFromString(QString xmlString)
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
          if(xml2Config(&xmlDevice))
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

    QVariant cReader::getValue(QString key)
    {
      Q_D(cReader);
      return d->data.value(key);
    }

    bool cReader::setValue(QString key, QVariant value)
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

    bool cReader::xml2Config(QIODevice *xmlData)
    {
      bool retVal = true;
      QXmlStreamReader xmlReader;
      QList<QString> parents;

      xmlReader.setDevice(xmlData);

      while(!xmlReader.atEnd() && !xmlReader.hasError())
      {
        QString fullPath;

        //read next
        QXmlStreamReader::TokenType token = xmlReader.readNext();

        // we read the actual data that stands between a start and an end node
        if(token == QXmlStreamReader::Characters)
        {
          QVariant nodeData;

          // ignore whitespaces
          if(!xmlReader.text().toString().isEmpty()&&!xmlReader.isWhitespace())
          {
            Q_D(cReader);
            nodeData=xmlReader.text().toString();

            //add the first parent if it exists, if not set fullPath to an empty QString
            parents.isEmpty() ? fullPath = "" : fullPath = parents.first();

            for(int pCount=1; pCount < parents.count(); pCount++)
            {
              fullPath = QString("%1:%2").arg(fullPath).arg(parents.at(pCount));
            }
            d->data.insert(fullPath, nodeData);
            valueChanged(fullPath);
          }
        }
        // add the node name as parent if it is a start node: <text>
        else if(token == QXmlStreamReader::StartElement)
        {
          parents.append(xmlReader.name().toString());
        }
        // remove the last node from the parents if it is and end node: </text>
        else if(token == QXmlStreamReader::EndElement)
        {
          parents.removeLast();
        }
      }

      /* Error handling. */
      if(xmlReader.hasError())
      {
        retVal = false;
        qDebug()<<"Error parsing XML: "<<xmlReader.errorString();
      }
      return retVal;
    }

  }
}
