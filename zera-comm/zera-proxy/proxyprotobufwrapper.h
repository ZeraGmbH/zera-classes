#ifndef PROXYPROTOBUFWRAPPER_H
#define PROXYPROTOBUFWRAPPER_H

#include <xiqnetwrapper.h>


class cProxyProtobufWrapper: public XiQNetWrapper
{
public:
    cProxyProtobufWrapper();

    google::protobuf::Message *byteArrayToProtobuf(QByteArray bA);
    QByteArray protobufToByteArray(google::protobuf::Message *pMessage);
};

#endif // PROXYPROTOBUFWRAPPER_H
