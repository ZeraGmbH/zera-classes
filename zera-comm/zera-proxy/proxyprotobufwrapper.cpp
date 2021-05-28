#include <netmessages.pb.h>

#include "proxyprotobufwrapper.h"


cProxyProtobufWrapper::cProxyProtobufWrapper()
{
}

std::shared_ptr<google::protobuf::Message> cProxyProtobufWrapper::byteArrayToProtobuf(QByteArray bA)
{
    ProtobufMessage::NetMessage *intermediate = new ProtobufMessage::NetMessage();
    if(!intermediate->ParseFromArray(bA, bA.size()))
    {
        ProtobufMessage::NetMessage::ScpiCommand *cmd = intermediate->mutable_scpi();
        cmd->set_command(bA.data(), bA.size() );
    }
    std::shared_ptr<google::protobuf::Message> proto {intermediate};
    return proto;
}
