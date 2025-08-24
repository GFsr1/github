#ifndef __M_WORKER_H__
#define __M_WORKER_H__

#include "muduo/net/EventLoopThread.h"
#include "../mqcommon/mq_logger.hpp"
#include "../mqcommon/mq_helper.hpp"
#include "../mqcommon/mq_threadpool.hpp"

namespace zkpmq {
    struct AsyncWorker {
        using ptr = std::shared_ptr<AsyncWorker>;
        muduo::net::EventLoopThread loopthread;
        threadpool pool;
    };
}

#endif /* __M_WORKER_H__ */
