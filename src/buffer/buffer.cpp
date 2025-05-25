#include "buffer.h"

Buffer::Buffer(int initBufferSize) : buffer_(initBufferSize), readPos_(0), writePos_(0) {}

/** 返回可读字节数 */
size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;
}

/** 返回可写字节数 */
size_t Buffer::WritableBytes() const {
    return buffer_.size() - writePos_;
}

/** 返回前置可用空间大小 */
size_t Buffer::PrependableBytes() const {
    return readPos_;
}

/** 获取可读数据的起始指针 */
const char* Buffer::Peek() const {
    return BeginPtr_() + readPos_;
}

/** 取出指定长度数据，移动读指针 */
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    readPos_ += len;
}

/** 取出直到end指针的数据 */
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end );
    Retrieve(end - Peek());
}

/** 清空缓冲区，重置读写指针 */
void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

/** 取出全部数据为字符串，并清空缓冲区 */
std::string Buffer::RetrieveAllToStr() {
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

/** 获取可写起始指针（常量） */
const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + writePos_;
}

/** 获取可写起始指针 */
char* Buffer::BeginWrite() {
    return BeginPtr_() + writePos_;
}

/** 写入数据后，移动写指针 */
void Buffer::HasWritten(size_t len) {
    writePos_ += len;
} 

/** 确保足够可写空间，不够则扩容 */
void Buffer::EnsureWritable(size_t len) {
    if(WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

/** 追加写入字符数组数据 */
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWritable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

/** 追加写入字符串数据 */
void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}

/** 追加写入任意数据 */
void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

/** 追加写入另一Buffer的数据 */
void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

/** 从文件描述符读取数据到缓冲区，支持分散读 */
ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = WritableBytes();
    /* 分散读，保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writePos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        writePos_ += len;
    }
    else {
        writePos_ = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

/** 将缓冲区数据写入文件描述符 */
ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readPos_ += len;
    return len;
}

/** 获取缓冲区起始指针 */
char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

/** 获取缓冲区起始指针（常量） */
const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

/** 扩容空间，保证有足够可写空间 */
void Buffer::MakeSpace_(size_t len) {
    // 扩容空间
    if(WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(writePos_ + len + 1);
    } 
    // 整理空间，未读数据前移
    else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == ReadableBytes());
    }
}