#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "blockqueue.h"
#include "../buffer/buffer.h"

class Log {
public:
    void init(int level, const char* path = "../../logs",
                const char* suffix = ".log",
                int maxQueueCapacity = 1024);
    
    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char* format, ...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }

private:
    Log();
    virtual ~Log();
    void AsyncWrite_();
    void AppendLogLevelTitle_(int level);

private:
    static const int LOG_PATH_LEN = 256;    // 路径最大长度
    static const int LOG_NAME_LEN = 256;    // 日志名最大长度
    static const int MAX_LINES = 50000;     // 单个日志文件最大行数

    const char* path_;      // 日志文件路径
    const char* suffix_;    // 日志文件后缀

    int MAX_LINES_;         // 最大行数
    int lineCount_;         // 行计数
    int toDay_;

    bool isOpen_;           // 是否打开
    bool isAsync_;           // 是否异步

    int level_;             // 日志级别
    Buffer buff_;           // 缓冲区

    FILE* fp_;              // 文件指针
    std::mutex mtx_;        // 互斥锁
    std::unique_ptr<std::thread> writeThread_;          // 写线程
    std::unique_ptr<BlockDeque<std::string>> deque_;    // 阻塞队列
};


/** 基础日志宏 */
#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if(log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__);\
            log->flush();\
        }\
    } while(0);

/** 不同级别的日志宏 */
#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);// 调试级别（0）
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0); // 信息级别（1）
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0); // 警告级别（2）
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);// 错误级别（3）

#endif  // LOG_H
