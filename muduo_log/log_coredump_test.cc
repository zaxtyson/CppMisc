#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>

using namespace std;
using namespace muduo;

AsyncLogging logger("coredump", 100 * 1000 * 1000);  // 分块 100MB

static void asyncOutput(const char *msg, int len)
{
    logger.append(msg, len);
}

int main(int argc, char const *argv[])
{
    logger.start();                 // 开启日志线程
    Logger::setOutput(asyncOutput); //设置日志输出函数

    for (int i = 0; i < 1000000; i++)
    {
        if (i == 1000000 / 2) abort();
        LOG_INFO << "This is a test message, msg_id=" << i;
    }
}
