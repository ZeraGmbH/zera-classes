#ifndef PROXYPROTOBUFWRAPPER_H
#define PROXYPROTOBUFWRAPPER_H

#include <xiqnetwrapper.h>


class cProxyProtobufWrapper: public XiQNetWrapper
{
public:
    cProxyProtobufWrapper();

    std::shared_ptr<google::protobuf::Message> byteArrayToProtobuf(QByteArray bA) override;
    QByteArray protobufToByteArray(const google::protobuf::Message &pMessage) override;
};

#endif // PROXYPROTOBUFWRAPPER_H
