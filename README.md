项目结构

```bash
WebServer/
├── build/                # 构建目录
│   ├── bin/              # 生成的可执行文件
│   └── obj/              # 各模块对象文件
├── docs/                 # 项目文档
├── logs/                 # 运行时日志
├── resources/            # 非代码资源
│   └── static/           # Web静态资源
├── src/                  # 主程序源代码
│   ├── main.cpp          # 入口文件
│   ├── server/  
│   ├── http/  
│   ├── pool/
│   ├── log/
│   ├── timer/
│   └── buffer/
├── test/                 # 测试框架
├── .gitignore
└── Makefile              # 主构建配置
```
