#include "mq_connection.hpp"

int main() {
    // 1. 实例化异步工作线程对象
    zkpmq::AsyncWorker::ptr awp = std::make_shared<zkpmq::AsyncWorker>();
    // 2. 实例化连接对象
    zkpmq::Connection::ptr conn = std::make_shared<zkpmq::Connection>("127.0.0.1", 8888, awp);
    // 3. 通过连接创建信道
    zkpmq::Channel::ptr channel = conn->openChannel();
    // 4. 通过信道提供的服务完成所需
    //   1. 声明一个交换机 exchange1，交换机类型为广播模式
    google::protobuf::Map<std::string, std::string>tmp_map;
    // channel->declareExchange("exchange1", zkpmq::ExchangeType::FANOUT, true, false, tmp_map);
    // channel->declareExchange("exchange1", zkpmq::ExchangeType::DIRECT, true, false, tmp_map);
    channel->declareExchange("exchange1", zkpmq::ExchangeType::TOPIC, true, false, tmp_map);
    //   2. 声明一个队列 queue1
    channel->declareQueue("queue1", true, false, false, tmp_map);
    //   3. 声明一个队列 queue2
    channel->declareQueue("queue2", true, false, false, tmp_map);
    //   4. 绑定 queue1-exchang1，且 binding_key 设置为 queue1
    channel->queueBind("exchange1", "queue1", "queue1");
    //   5. 绑定 queue2-exchang1，且 binding_key 设置为 news.music.#
    channel->queueBind("exchange1", "queue2", "news.music.#");
    // 5. 循环向交换机发布消息
    for (int i = 0; i < 10; i++) {
        // channel->basicPublish("exchange1", nullptr, "Hello World-" + std::to_string(i));
        zkpmq::BasicProperties bp;
        bp.set_id(zkpmq::UUIDHelper::uuid());
        bp.set_delivery_mode(zkpmq::DeliveryMode::DURABLE);
        bp.set_routing_key("news.music.pop");
        channel->basicPublish("exchange1", &bp, "Hello World-" + std::to_string(i));
    }
    zkpmq::BasicProperties bp;
    bp.set_id(zkpmq::UUIDHelper::uuid());
    bp.set_delivery_mode(zkpmq::DeliveryMode::DURABLE);
    bp.set_routing_key("news.music.sport");
    channel->basicPublish("exchange1", &bp, "Hello zkp");

    
    bp.set_routing_key("news.sport");
    channel->basicPublish("exchange1", &bp, "Hello chileme?");
    // 6. 关闭信道
    conn->closeChannel(channel);
    return 0;
}