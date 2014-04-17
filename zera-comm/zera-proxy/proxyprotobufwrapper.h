#ifndef PROXYPROTOBUFWRAPPER_H
#define PROXYPROTOBUFWRAPPER_H

#include <protonetwrapper.h>


class cProxyProtobufWrapper: public ProtoNetWrapper
{
public:
    cProxyProtobufWrapper();

    google::protobuf::Message *byteArrayToProtobuf(QByteArray bA);
    QByteArray protobufToByteArray(google::protobuf::Message *pMessage);
};

#endif // PROXYPROTOBUFWRAPPER_H
