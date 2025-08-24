#include "../mqserver/mq_channel.hpp"

int main() {
    zkpmq::ChannelManager::ptr cmp = std::shared_ptr<zkpmq::ChannelManager>();

    cmp->openChannel("c1",
        std::make_shared<zkpmq::VirtualHost>("host1", "./data/host1/message/", "./data/host1/host1.db"), 
        std::make_shared<zkpmq::ConsumerManager>(),
        zkpmq::ProtobufCodecPtr(),
        muduo::net::TcpConnectionPtr(),
        threadpool::ptr());
    return 0;
}