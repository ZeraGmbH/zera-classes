#include "xmlconfigreader_private.h"

#include <QXmlStreamReader>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QtXmlPatterns/QXmlSchema>
#include <QStringList>
#include <QByteArray>
#include <QFile>

#include <QDebug>

namespace Zera
{
  namespace XMLConfig
  {
    _XMLConfigReaderPrivate::_XMLConfigReaderPrivate(QObject *parent) :
      QObject(parent)
    {
      schemaFilePath=QString();
    }

    bool _XMLConfigReaderPrivate::loadSchema(const QString &filePath)
    {
      data = schema2Config(filePath);
      return !(data.isEmpty());
    }



    bool _XMLConfigReaderPrivate::loadXML(const QString &filePath)
    {
      bool retVal = false;
      QXmlSchema schema;
      QFile schemaFile(schemaFilePath);
      schemaFile.open(QFile::ReadOnly);

      if(schema.load(&schemaFile,QUrl(schemaFilePath)))
      {
        QXmlSchemaValidator sValidator(schema);

        //qDebug() << "schema is valid";

        if(sValidator.validate(QUrl(QString("file://%1").arg(filePath))))
        {
          //qDebug() << "XML is valid";
          if(xml2Config(filePath))
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

    QVariant _XMLConfigReaderPrivate::getValue(const QString &key)
    {
      return data.value(key);
    }

    bool _XMLConfigReaderPrivate::setValue(const QString &key, QVariant value)
    {
      bool retVal=false;
      if(data.contains(key))
      {
        data.insert(key, value);
        retVal=true;
      }
      return retVal;
    }


    QString _XMLConfigReaderPrivate::getXMLConfig()
    {
      QString retVal = QString();
      QStringList parents, oldParents;
      QXmlStreamWriter stream(&retVal);
      stream.setAutoFormatting(true);
      stream.writeStartDocument();

      // for each key in the QMap
      for(int elemCount=0; elemCount<data.keys().count(); elemCount++)
      {
        QString elementName;
        parents = data.keys().at(elemCount).split(":");
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
        stream.writeTextElement(elementName,data.values().at(elemCount).toString());
      }
      stream.writeEndDocument();

      return retVal;
    }


    QHash<QString, QVariant> _XMLConfigReaderPrivate::schema2Config(const QString &xsdPath)
    {
      QFile *schemaFile=0;
      QXmlStreamReader xmlReader;
      QHash<QString, QVariant>retVal;
      QList<QString> parents;

      schemaFilePath = xsdPath;

      schemaFile = new QFile(xsdPath);
      schemaFile->open(QFile::ReadOnly);

      xmlReader.setDevice(schemaFile);

      while(!xmlReader.atEnd() && !xmlReader.hasError())
      {
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument)
        {
          continue;
        }
        /* If token is StartElement, we'll see if we can read it.*/
        else if(token == QXmlStreamReader::StartElement && xmlReader.name()=="element")
        {
          QString elementName = xmlReader.attributes().value("name").toString();
          QString fullPath;

          //add the first parent if it exists, if not set fullPath to an empty QString
          parents.isEmpty() ? fullPath = "" : fullPath = parents.first();

          for(int pCount=1; pCount<parents.count(); pCount++)
          {
            fullPath = QString("%1:%2").arg(fullPath).arg(parents.at(pCount));
          }

          // only add a colon if there are parent elements
          (fullPath=="") ? fullPath=elementName : fullPath = fullPath.append(QString(":%1").arg(elementName));

          // do not add envelope nodes that has no value itself
          if(xmlReader.attributes().hasAttribute("type"))
          {
            retVal.insert(fullPath, QVariant(QVariant::Invalid));
          }
          parents.append(elementName);
        }
        else if(token == QXmlStreamReader::EndElement && xmlReader.name()=="element")
        {
          parents.removeLast();
        }
      }
      /* Error handling. */
      if(xmlReader.hasError())
      {
        retVal = QHash<QString, QVariant>();
        qDebug()<<"Error parsing xsd file: " << xmlReader.errorString();
      }

      schemaFile->deleteLater();
      return retVal;
    }

    bool _XMLConfigReaderPrivate::xml2Config(const QString &xmlPath)
    {
      bool retVal = true;
      QFile *xmlFile = 0;
      QXmlStreamReader xmlReader;
      QList<QString> parents;


      xmlFile = new QFile(xmlPath);
      xmlFile->open(QFile::ReadOnly);

      xmlReader.setDevice(xmlFile);

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
            nodeData=xmlReader.text().toString();

            //add the first parent if it exists, if not set fullPath to an empty QString
            parents.isEmpty() ? fullPath = "" : fullPath = parents.first();

            for(int pCount=1; pCount < parents.count(); pCount++)
            {
              fullPath = QString("%1:%2").arg(fullPath).arg(parents.at(pCount));
            }
            data.insert(fullPath, nodeData);
            emit valueChanged(fullPath);
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

      xmlFile->deleteLater();
      return retVal;
    }
  }
}
