#include "xmlconfigreaderprivate.h"
#include "xmlconfigreader.h"

#include <QXmlStreamReader>
#include <QStringList>
#include <QByteArray>
#include <QFile>

#include <QDebug>

namespace Zera
{
  namespace XMLConfig
  {
    ReaderPrivate::ReaderPrivate()
    {
      schemaFilePath=QString();
    }



    QHash<QString, QVariant> ReaderPrivate::schema2Config(const QString &xsdPath)
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

    bool ReaderPrivate::xml2Config(QIODevice *xmlData)
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
            Q_Q(Reader);
            nodeData=xmlReader.text().toString();

            //add the first parent if it exists, if not set fullPath to an empty QString
            parents.isEmpty() ? fullPath = "" : fullPath = parents.first();

            for(int pCount=1; pCount < parents.count(); pCount++)
            {
              fullPath = QString("%1:%2").arg(fullPath).arg(parents.at(pCount));
            }
            data.insert(fullPath, nodeData);
            q->valueChanged(fullPath);
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
