## 1. log4cpp 是什么

**log4cpp** 是一个 C++ 日志库，设计思想借鉴了 Java 的 Log4j。它提供一套可配置的日志系统，可以把日志输出到控制台、文件、滚动文件、syslog、IDSA、SMTP、Windows Event Log 等目标。官方 README 也明确列出它支持多日志级别、线程安全日志、自定义格式、无外部依赖、跨 Linux/macOS/Windows，并支持 Autotools 和 CMake 构建。

截至官方页面当前信息，log4cpp 的最新稳定版是 **1.1.6**；项目自 1.1.1 起迁移到 Git，旧 CVS 仓库已经不再维护。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/)) 它采用 **LGPL-2.1** 许可证。

它适合解决这些问题：

- 不想在代码里到处写 `printf` / `std::cout`。
- 希望按级别输出日志，例如 `DEBUG`、`INFO`、`WARN`、`ERROR`。
- 希望同一条日志同时输出到控制台和文件。
- 希望日志格式统一，例如时间、线程、模块名、级别、消息。
- 希望通过配置文件调整日志行为，而不是重新编译程序。
- 希望日志文件自动滚动，避免单个日志文件无限增长。

需要特别说明：**log4cpp 不是 Java Log4j**，只是 API 和设计思想接近 Log4j。它是 C++ 库，不应和 Java Log4j 的运行时机制混为一谈。

------

## 2. 核心架构：Category + Appender + Layout

log4cpp 的核心模型可以理解为：

```text
你的代码
  ↓
Category / Logger
  ↓  根据日志级别判断是否输出
Appender
  ↓  决定输出到哪里
Layout
  ↓  决定日志长什么样
控制台 / 文件 / 滚动文件 / syslog / Windows Event Log ...
```

### 2.1 Category：日志记录器

`Category` 是最核心的类，基本等价于很多日志框架里的 `Logger`。官方 API 说明中也称它是 log4j/log4cpp 包里的中心类，并强调 log4cpp 支持层级化 category。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1Category.html))

常见用法：

```cpp
log4cpp::Category& root = log4cpp::Category::getRoot();
log4cpp::Category& netLogger = log4cpp::Category::getInstance("network");
log4cpp::Category& dbLogger = log4cpp::Category::getInstance("database");
```

`Category` 有层级关系，例如：

```text
root
 ├── network
 │    └── network.http
 └── database
      └── database.mysql
```

这带来几个好处：

- 可以给不同模块设置不同日志级别。
- 可以让子 logger 继承父 logger 的配置。
- 可以让某个模块单独输出到自己的文件。
- 可以控制日志是否向父 category 传播，也就是 additivity。

官方 API 中 `getRoot()` 用于获取根 category，`getInstance(name)` 用于按名称获取或创建 category；未显式设置 priority 的 category 默认是 `NOTSET`，会通过层级查找有效 priority。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1Category.html))

------

### 2.2 Priority：日志级别

log4cpp 支持这些日志级别，从低到高大致是：

```text
TRACE
DEBUG
INFO
NOTICE
WARN
ERROR
CRIT
ALERT
FATAL
```

官方文档列出了这些 priority，并解释了它们的典型用途：`TRACE` 用于非常细粒度的诊断，`DEBUG` 用于调试，`INFO` 用于普通运行信息，`WARN` 表示可恢复异常，`ERROR` 表示操作失败，`FATAL` 表示严重到可能导致程序终止的问题。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/))

一般项目中常用的是：

```text
DEBUG：开发调试信息
INFO ：正常业务流程信息
WARN ：潜在问题，但程序还能继续
ERROR：某个操作失败
FATAL：严重错误，程序可能无法继续
```

例如：

```cpp
logger.debug("debug message");
logger.info("server started");
logger.warn("disk usage is high");
logger.error("failed to open config file");
logger.fatal("cannot initialize core service");
```

日志输出会受 category 级别影响。比如 category 设置为 `INFO`，通常 `DEBUG` 日志不会输出，而 `INFO`、`WARN`、`ERROR` 会输出。

------

### 2.3 Appender：日志输出目的地

`Appender` 决定日志输出到哪里。官方 API 说明中，Appender 是“打印日志语句策略”的接口，它支持设置 layout、threshold、filter，也支持 reopen/close 等操作。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1Appender.html))

常见 Appender：

| Appender                   | 用途                                 |
| -------------------------- | ------------------------------------ |
| `OstreamAppender`          | 输出到 `std::cout`、`std::cerr` 等流 |
| `FileAppender`             | 输出到普通文件                       |
| `RollingFileAppender`      | 文件达到一定大小后滚动               |
| `DailyRollingFileAppender` | 按日期滚动                           |
| `SyslogAppender`           | 输出到本地 syslog                    |
| `RemoteSyslogAppender`     | 输出到远程 syslog                    |
| `StringQueueAppender`      | 输出到内存字符串队列                 |
| `NTEventLogAppender`       | 输出到 Windows Event Log             |
| `Win32DebugAppender`       | 输出到 Windows 调试器                |

其中最常用的是：

```text
OstreamAppender       控制台日志
FileAppender          普通文件日志
RollingFileAppender   滚动文件日志
```

`RollingFileAppender` 是 `FileAppender` 的子类，官方说明它会在日志文件达到指定大小时执行 rollover；其构造函数默认 `maxFileSize = 10 * 1024 * 1024`，`maxBackupIndex = 1`。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1RollingFileAppender.html))

------

### 2.4 Layout：日志格式

`Layout` 决定一条日志最终长什么样。最常用的是 `PatternLayout`。

官方文档说明 `PatternLayout` 用 `setConversionPattern()` 设置日志格式，默认格式是 `%m%n`，即只输出消息和换行。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1PatternLayout.html))

常用格式符：

| 符号                       | 含义                          |
| -------------------------- | ----------------------------- |
| `%m`                       | 日志消息                      |
| `%n`                       | 换行                          |
| `%p`                       | 日志级别                      |
| `%c`                       | category 名称                 |
| `%d`                       | 日期时间                      |
| `%d{%Y-%m-%d %H:%M:%S,%l}` | 自定义日期格式，`%l` 表示毫秒 |
| `%r`                       | layout 创建以来的毫秒数       |
| `%R`                       | Unix 时间戳秒数               |
| `%u`                       | 进程启动以来的 clock ticks    |
| `%x`                       | NDC，上下文信息               |
| `%t`                       | 线程名                        |

例如：

```text
%d{%Y-%m-%d %H:%M:%S,%l} [%p] %c - %m%n
```

输出可能类似：

```text
2026-05-07 14:20:31,123 [INFO] network.http - request completed
```

------

## 3. 安装与构建

log4cpp 支持 Autotools 和 CMake。官方 README 给出的 Autotools 构建方式是：

```bash
./autogen.sh  # optional
./configure
make
make check
make install
```

CMake 构建方式是：

```bash
cmake -S . -B build_release \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$HOME/.local \
  -DLOG4CPP_BUILD_EXAMPLES=ON

cmake --build build_release
cd build_release/tests
ctest
cd ../..
cmake --install build_release
```

官方 README 还说明了两种 CMake 使用方式：作为已安装的 CMake package 导入，或者作为 CMake subproject 使用。([GitHub](https://github.com/log4cpp/log4cpp))

在 Linux 上，如果系统包管理器提供了 log4cpp，也可以直接安装，例如某些发行版可能提供：

```bash
sudo apt install liblog4cpp5-dev
```

不过具体包名和版本取决于发行版仓库。

------

## 4. 最小代码示例：手动配置

下面是一个比较典型的手动配置例子：同时输出到控制台和滚动文件。

```cpp
#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

int main() {
    // 控制台 Appender
    auto* consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
    auto* consoleLayout = new log4cpp::PatternLayout();
    consoleLayout->setConversionPattern(
        "%d{%Y-%m-%d %H:%M:%S,%l} [%p] %c - %m%n"
    );
    consoleAppender->setLayout(consoleLayout);

    // 滚动文件 Appender：10MB 一个文件，最多保留 5 个备份
    auto* fileAppender = new log4cpp::RollingFileAppender(
        "rollingFile",
        "app.log",
        10 * 1024 * 1024,
        5
    );
    auto* fileLayout = new log4cpp::PatternLayout();
    fileLayout->setConversionPattern(
        "%d{%Y-%m-%d %H:%M:%S,%l} [%p] %c - %m%n"
    );
    fileAppender->setLayout(fileLayout);

    // root logger
    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::INFO);

    // 注意：addAppender(Appender*) 会把所有权交给 Category
    root.addAppender(consoleAppender);
    root.addAppender(fileAppender);

    root.debug("this debug message will not be printed");
    root.info("server started, port=%d", 8080);
    root.warnStream() << "memory usage is high: " << 92 << "%";
    root.error("failed to open file: %s", "config.yaml");

    log4cpp::Category::shutdown();
    return 0;
}
```

这里有一个容易踩坑的点：`Category::addAppender(Appender*)` 会把 Appender 所有权交给 Category，而 `addAppender(Appender&)` 不会转移所有权。官方 API 对这两个重载分别做了说明。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1Category.html))

------

## 5. 推荐方式：使用配置文件

在实际项目中，更推荐用配置文件，因为你可以不改代码就调整日志级别、日志路径、滚动策略和格式。log4cpp 的 `PropertyConfigurator` 会读取类似 log4j 的 properties 配置文件；官方 API 说明它使用标准 Java properties 文件格式。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1PropertyConfigurator.html))

### 5.1 C++ 代码

```cpp
#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

int main() {
    try {
        log4cpp::PropertyConfigurator::configure("log4cpp.properties");
    } catch (const log4cpp::ConfigureFailure& ex) {
        std::cerr << "log4cpp configure failed: " << ex.what() << std::endl;
        return 1;
    }

    log4cpp::Category& root = log4cpp::Category::getRoot();
    log4cpp::Category& network = log4cpp::Category::getInstance("network");
    log4cpp::Category& database = log4cpp::Category::getInstance("database");

    root.info("application started");
    network.debug("connect to server");
    database.error("query failed: %s", "timeout");

    log4cpp::Category::shutdown();
    return 0;
}
```

### 5.2 `log4cpp.properties`

```properties
# root logger：INFO 级别，输出到 console 和 rollingFile
log4cpp.rootCategory=INFO, console, rollingFile

# network 模块单独设置为 DEBUG
log4cpp.category.network=DEBUG, console, rollingFile

# database 模块设置为 WARN
log4cpp.category.database=WARN, console, rollingFile

# 控制台输出
log4cpp.appender.console=ConsoleAppender
log4cpp.appender.console.layout=PatternLayout
log4cpp.appender.console.layout.ConversionPattern=%d{%Y-%m-%d %H:%M:%S,%l} [%p] %c - %m%n

# 滚动文件输出
log4cpp.appender.rollingFile=RollingFileAppender
log4cpp.appender.rollingFile.fileName=app.log
log4cpp.appender.rollingFile.maxFileSize=10485760
log4cpp.appender.rollingFile.maxBackupIndex=5
log4cpp.appender.rollingFile.layout=PatternLayout
log4cpp.appender.rollingFile.layout.ConversionPattern=%d{%Y-%m-%d %H:%M:%S,%l} [%p] %c - %m%n
```

官方示例也展示了 `PropertyConfigurator::configure()`、`log4cpp.rootCategory`、`log4cpp.category.*`、`FileAppender`、`RollingFileAppender`、`maxFileSize`、`maxBackupIndex` 和 `ConversionPattern` 的组合用法。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/))

------

## 6. 日志级别过滤机制

log4cpp 中有两层常见过滤：

第一层是 **Category priority**。例如：

```properties
log4cpp.rootCategory=INFO, console
```

这意味着 root 下低于 `INFO` 的日志一般不会输出。

第二层是 **Appender threshold**。例如：

```properties
log4cpp.appender.errorFile=FileAppender
log4cpp.appender.errorFile.threshold=ERROR
log4cpp.appender.errorFile.fileName=error.log
```

这样可以做到：

```text
console     输出 INFO 及以上
app.log     输出 DEBUG 及以上
error.log   只输出 ERROR 及以上
```

Appender API 中也明确有 `setThreshold()` / `getThreshold()`，并说明低于 appender threshold 的 LoggingEvent 不会被该 appender 输出。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/api/classlog4cpp_1_1Appender.html))

------

## 7. 线程安全与注意事项

log4cpp 支持多线程日志。官方 FAQ 说明，同一个 `log4cpp::Category` 实例可以被多个线程同时使用，不需要显式同步；category 在写 appender 时会锁内部 mutex，因此多线程写同一个 logger 时日志记录不会互相交错。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/))

但也有一个重要限制：如果你配置成多个不同 logger 共享同一个 appender 实例，框架不能保证这些 logger 之间的同步，日志可能交错或损坏；官方也不推荐多个 logger 共享同一个 appender 实例。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/))

实践建议：

```text
推荐：
一个 category 拥有自己的 appender，或通过配置文件统一管理。

谨慎：
多个 category 手动共享同一个 appender 指针。

避免：
同一个 appender 被多个 category 重复接管所有权，可能引发释放问题。
```

------

## 8. 性能相关

log4cpp 支持普通函数式日志、stream 风格日志和宏。官方文档列出三种写日志方式：直接调用 `debug()` / `info()` 等方法，使用 C++ stream，或者使用宏。宏的优势是：当日志级别低于当前 logger 级别时，参数不会被求值，从而避免不必要的计算。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/))

例如：

```cpp
LOG4CPP_DEBUG(logger, "expensive result: %d", expensiveFunction());
LOG4CPP_INFO_S(logger, "user id: " << userId);
```

如果 `DEBUG` 没启用，`expensiveFunction()` 不会被调用。

还可以通过 `LOG4CPP_ACTIVE_LEVEL` 在编译期移除低级别日志，例如官方文档给出的思路是定义：

```cpp
#define LOG4CPP_ACTIVE_LEVEL LOG4CPP_PRIORITY_INFO
#include <log4cpp/LogMacros.hh>
```

这样可以在编译期去掉低于 `INFO` 的日志宏，进一步降低运行时开销。([log4cpp.sourceforge.net](https://log4cpp.sourceforge.net/))

------

## 9. NDC：嵌套诊断上下文

`NDC` 是 Nested Diagnostic Context，常用于多线程服务、请求追踪、会话追踪等场景。你可以把 request id、user id、connection id 等上下文放入 NDC，然后在 `PatternLayout` 中用 `%x` 输出。

例如格式中包含：

```properties
log4cpp.appender.console.layout.ConversionPattern=%d [%p] %c %x - %m%n
```

这样可以让交错出现的日志更容易区分：

```text
2026-05-07 14:22:01 [INFO] api req-1001 - start request
2026-05-07 14:22:01 [INFO] api req-1002 - start request
2026-05-07 14:22:02 [INFO] api req-1001 - finish request
```

这对服务端程序很有用。

------

## 10. 优点、缺点与适用场景

### 优点

log4cpp 的优点很明显：

- API 和 Log4j 风格接近，概念成熟。
- 支持 category 层级，适合中大型 C++ 项目。
- 支持配置文件，方便生产环境动态调整日志级别。
- 支持控制台、普通文件、滚动文件、syslog、Windows Event Log 等输出目标。
- 无外部依赖，跨平台。
- 同一个 category 多线程写日志是安全的。([GitHub](https://github.com/log4cpp/log4cpp))

### 缺点

它的不足也要注意：

- API 风格偏传统，和现代 C++ 日志库相比没有那么轻量。
- 使用裸指针配置 appender/layout 时，要注意所有权语义。
- 默认不是结构化日志库，JSON、trace id、span id 等现代可观测性能力通常需要自己封装。
- 如果追求极致性能、header-only、异步日志、现代格式化体验，可以同时评估 spdlog；spdlog 官方 README 明确支持 header-only/compiled、异步模式、fmt 风格格式化、多种 sink 等特性。([GitHub](https://github.com/gabime/spdlog?utm_source=chatgpt.com))

### 适合使用 log4cpp 的场景

比较适合：

```text
已有项目已经使用 log4cpp
需要 log4j 风格配置文件
需要稳定、传统、无外部依赖的 C++ 日志库
项目主要关注文件日志、滚动日志、syslog、Windows Event Log
```

不一定最适合：

```text
全新高性能 C++17/C++20 项目
强依赖异步日志
强依赖结构化 JSON 日志
希望使用 header-only 库
希望日志库和现代 fmt 格式化深度集成
```

------

## 11. 实际项目最佳实践

推荐这样用：

1. **生产环境用配置文件，不要硬编码日志行为。**

   代码只写：

   ```cpp
   log4cpp::PropertyConfigurator::configure("log4cpp.properties");
   ```

   级别、路径、格式、滚动策略都放到配置文件里。

2. **按模块创建 category。**

   例如：

   ```cpp
   log4cpp::Category& net = log4cpp::Category::getInstance("network");
   log4cpp::Category& db = log4cpp::Category::getInstance("database");
   log4cpp::Category& auth = log4cpp::Category::getInstance("auth");
   ```

3. **生产环境默认 `INFO` 或 `WARN`。**

   `DEBUG` 日志可以保留，但不要默认打开。

4. **文件日志优先用 `RollingFileAppender`。**

   避免日志文件无限增长。

5. **错误日志可以单独输出。**

   例如：

   ```text
   app.log    保存 INFO 及以上
   error.log  保存 ERROR 及以上
   ```

6. **不要记录敏感信息。**

   密码、token、密钥、身份证号、银行卡号等不要直接写入日志。

7. **程序退出前调用 shutdown。**

   ```cpp
   log4cpp::Category::shutdown();
   ```

8. **高频日志使用宏。**

   避免日志级别关闭时仍然构造复杂字符串或调用昂贵函数。

------

## 12. 一句话总结

**log4cpp 是一个传统、成熟、Log4j 风格的 C++ 日志库，核心由 `Category`、`Appender`、`Layout`、`Priority`、`PropertyConfigurator` 组成。它适合需要配置化、多目的地、滚动文件、跨平台日志的 C++ 项目；如果是新项目并且追求现代 C++、异步、高性能或结构化日志，则建议同时对比 spdlog、log4cplus、log4cxx 等方案。**