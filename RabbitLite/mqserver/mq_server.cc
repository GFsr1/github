#include "mq_broker.hpp"

int main() {
    zkpmq::Server server(8888, "./data/");
    server.start();
    return 0;
}