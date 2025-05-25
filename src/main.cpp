#include <unistd.h>
#include "server/webserver.h"

int main() {

    WebServer server(
        1303,           // 端口 1024~65535
        3,              // ET模式
        60000,          // timeoutMs
        false,          // 优雅退出

        /* Mysql配置 */
        1312,           // SQL端口
        "JIN",          // SQL用户
        "123456",       // SQL密码
        "webserv",      // 数据库名

        12,             // 连接池连接数量
        6,              // 线程池线程数量
        true,           // 日志开关
        1,              // 日志等级
        1024            // 日志异步队列容量
    );

    server.Start();
    return 0;
}