#ifndef PROXYPROTOBUFWRAPPER_H
#define PROXYPROTOBUFWRAPPER_H

#include <memory>
#include <xiqnetwrapper.h>


class cProxyProtobufWrapper: public XiQNetWrapper
{
public:
    cProxyProtobufWrapper();

    std::shared_ptr<google::protobuf::Message> byteArrayToProtobuf(QByteArray bA) override;
};

#endif // PROXYPROTOBUFWRAPPER_H
