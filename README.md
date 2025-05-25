# WebServer

用C++实现的高性能 HTTP 服务器，经过 webbench 压力测试可以实现上万的 QPS。

## 功能

- 使用线程池 + 非阻塞 socket + epoll(ET)+ Reactor 事件处理模式的并发模型；
- 利用正则与状态机解析 HTTP 请求报文，实现处理静态资源的 GET 和 POST 请求；
- 利用标准库容器封装 char，实现自动增长的缓冲区；
- 基于小根堆实现的定时器，关闭超时的非活动连接；
- 利用单例模式与阻塞队列实现异步的日志系统，记录服务器运行状态；
* 利用 RAII 机制实现了数据库连接池，减少数据库连接建立与关闭的开销，实现了用户注册登录功能。

## 环境要求

* Linux
* C++14
* MySQL

## 项目结构

```bash
WebServer/
├── build/                # 构建目录
│   ├── bin/              # 可执行文件
│   └── obj/              # 各模块对象文件
├── docs/                 # 项目文档
├── logs/                 # 运行日志
├── resources/            # 非代码资源
│   └── static/           # Web静态资源
├── src/                  # 程序源代码
│   ├── main.cpp          # 入口文件
│   ├── server/  
│   ├── http/  
│   ├── pool/
│   ├── log/
│   ├── timer/
│   └── buffer/
├── test/                 # 单元测试
├── webbench-1.5/         # 压力测试
├── .gitignore
├── Makefile              # 构建配置
└── README.md
```

## 项目启动

需要先配置好对应的数据库

```bash
// 建立webserv库
create database webserv;

// 创建user表
USE webserv;
CREATE TABLE user(
    username char(50) NULL,
    password char(50) NULL
)ENGINE=InnoDB;

// 添加数据
INSERT INTO user(username, password) VALUES('name', 'password');
```

构建并运行服务器

```bash
make
./build/bin/server
```

客户端访问

```bash
http://<ip>:<port>/
```

## 压力测试

webbench 压测结果

```bash
$ ./webbench -c 10500 -t 5 http://192.168.199.128:1303/
$ Webbench - Simple Web Benchmark 1.5
$ Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

$ Benchmarking: GET http://192.168.199.128:1303/
$ 10500 clients, running 5 sec.

$ Speed=344724 pages/min, 764138 bytes/sec.
$ Requests: 28727 susceed, 0 failed.
```

## 致谢

1. 《Linux高性能服务器编程》，游双著.

2. [TinyWebServer@qinguoyi](https://github.com/qinguoyi/TinyWebServer)

3. [WebServer@markparticle](https://github.com/markparticle/WebServer)