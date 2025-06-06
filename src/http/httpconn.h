#ifndef HTTPCONN_H
#define HTTPCONN_H

#include <errno.h>
#include <stdlib.h>     // atoi()
#include <sys/uio.h>    // readv, writev
#include <arpa/inet.h>  // sockaddr_in
#include <sys/types.h>

#include "httprequest.h"
#include "httpresponse.h"
#include "../log/log.h"
#include "../buffer/buffer.h"
#include "../pool/sqlconnRAII.h"

class HttpConn {
public:
    HttpConn();
    ~HttpConn();

    void init(int sockFd, const sockaddr_in& addr);

    ssize_t read(int* saveErrno);
    ssize_t write(int* saveErrno);

    void Close();
    bool process();
    int GetFd() const;
    int GetPort() const;
    const char* GetIP() const;
    sockaddr_in GetAddr() const;

    int ToWriteBytes() { 
        return iov_[0].iov_len + iov_[1].iov_len; 
    }

    bool IsKeepAlive() const {
        return request_.IsKeepAlive();
    }

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;

private:
    int fd_;
    struct sockaddr_in addr_;

    bool isClose_;
    
    int iovCnt_;
    struct iovec iov_[2];
    
    Buffer readBuff_;   // 读缓冲区
    Buffer writeBuff_;  // 写缓冲区

    HttpRequest request_;
    HttpResponse response_;
};

#endif  // HTTPCONN_H