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
    cReaderPrivate::cReaderPrivate()
    {
      schemaFilePath=QString();
    }

    bool cReaderPrivate::xml2Config(QIODevice *xmlData)
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
            Q_Q(cReader);
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
