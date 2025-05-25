#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <unistd.h>     // write
#include <sys/uio.h>    // readv
#include <vector>
#include <atomic>
#include <cstring>
#include <cassert>

class Buffer {
public:
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;

    size_t ReadableBytes() const;
    size_t WritableBytes() const;
    size_t PrependableBytes() const;


    const char* Peek() const;
    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);
    void RetrieveAll() ;
    std::string RetrieveAllToStr();


    const char* BeginWriteConst() const;
    char* BeginWrite();
    void HasWritten(size_t len);
    void EnsureWritable(size_t len);

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);


    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

    
private:
    char *BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSpace_(size_t len);

private:
    std::vector<char> buffer_;              // 数据缓冲区
    std::atomic<std::size_t> readPos_;      // 可读起始位置
    std::atomic<std::size_t> writePos_;     // 可写起始位置
};

#endif  // BUFFER_H