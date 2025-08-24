#include "../mqserver/mq_connection.hpp"

int main() {
    zkpmq::ConnectionManager::ptr cmp = std::make_shared<zkpmq::ConnectionManager>();
    cmp->newConnection(std::make_shared<zkpmq::VirtualHost>("host1", "./data/host1/message/", "./data/host1/host1.db"),
        std::make_shared<zkpmq::ConsumerManager>(),
        zkpmq::ProtobufCodecPtr(),
        muduo::net::TcpConnectionPtr(),
        threadpool::ptr());
    return 0;
}
