#include <netmessages.pb.h>

#include "proxyprotobufwrapper.h"


cProxyProtobufWrapper::cProxyProtobufWrapper()
{
}

google::protobuf::Message *cProxyProtobufWrapper::byteArrayToProtobuf(QByteArray bA)
{
    ProtobufMessage::NetMessage *proto = new ProtobufMessage::NetMessage();
    if(!proto->ParseFromArray(bA, bA.size()))
    {
        ProtobufMessage::NetMessage::ScpiCommand *cmd = proto->mutable_scpi();
        cmd->set_command(bA.data(), bA.size() );
    }
    return proto;
}


QByteArray cProxyProtobufWrapper::protobufToByteArray(google::protobuf::Message *pMessage)
{
    return QByteArray(pMessage->SerializeAsString().c_str(), pMessage->ByteSize());
}
