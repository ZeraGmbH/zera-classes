#include "xmlconfigreaderprivate.h"
#include "xmlconfigreader.h"

namespace Zera
{
namespace XMLConfig
{

cReaderPrivate::cReaderPrivate(cReader *parent) :
    q_ptr(parent)
{
    schemaFilePath=QString();
}

}
}
