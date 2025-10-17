# ColloLog

ColloLog is a lightweight, high-performance C++ logging library designed for multithreaded and performance-critical applications. It offers efficient, thread-safe logging with minimal performance impact.

![License: MIT](https://img.shields.io/badge/license-MIT-blue) ![Platform: Windows](https://img.shields.io/badge/platform-Windows-blue)

---

## Why use ColloLog?

`ColloLog` is designed for general-purpose and performance logging in multithreaded environments. It offers:

- Plain text output (for readability)

- Timestamps (program's runtime timer for efficiency)

- Static format (for optimization)

**For single-threaded projects**, ``ColloLog`` is still highly performant by offering a throughput of **16'000'000 logs/s** which is the fastest plain-text logger I know of.

### So when to use this tool?

- High-performance systems

- Multithreaded or concurrent systems

- Benchmarking

- Low disk foot-print requirements

## Adapting to your project

`ColloLog` provides two logger variants, optimized for different threading models:

- `collog`: highly performant for **few threads** (1-3)

- `localog`: highly performant for **multiple threads** (2+)

**Throughput example:** Logs were automatically flushed to disk by the tool and messages were static.

- [1 thread] `collog`: 16'000'000 logs/s 

- [1 thread] `localog`: 11'000'000 logs/s 

- [2 threads] `collog` : 14'5000'000 logs/s

- [2+ threads] `localog`: 17'500'000 logs/s

> :exclamation: Benchmarks were run on an Intel i7-13620H (13th gen), 32GB RAM.

> :warning: Beware: benchmarks do not replicate normal use in an application has applications do not spam the logger every 50-100 nanoseconds per thread. Therefore, `collog` is still the prefered tool when using 3-4 regularly threads.

---

## About Collineo

Collineo Inc. is committed to delivering top-quality solutions to its clients. With expertise in automation machinery and software development, Collineo serves a diverse range of industries, leveraging adaptability as one of its greatest strengths.

Link to our [LinkedIn][1]!

[1]: https://ca.linkedin.com/company/collineo-inc

---

## Installation

- In the root folder of your project, create a folder named `external` and change to that directory.

- Add the library as a submodule:

```bash
git submodule add -b main https://github.com/Arnaud-Gagner/ColloLog.git ColloLog
```

- Update the library:

```bash
git submodule update --init --recursive
```

- Build the library:

Make sure you are in the root folder.

```bash
mkdir build;
cd build;
cmake .. -DCMAKE_BUILD_TYPE=Release;
cmake --build . --config Release;
cmake --install . --prefix install --config Release;
cmake .. -DCMAKE_BUILD_TYPE=Debug;
cmake --build . --config Debug;
cmake --install . --prefix install --config Debug;
```

- Link it with your build setup

## Integrating to your projects

**1. Instantiate** a logger in the main.cpp. There are two `FileOpen` options:
    - `FileOpen::Append`: default value, does not clear the file when opening.
    - `FileOpen::Clear`: clears the file when opening.

```c
#include <ColloLog.h>

collog::init("path/to/logs");
```

**2. Flushing strategies:** By default, `ColloLog` will automatically flush data when its buffer is full. When calling a logging function, it can take an extra arguments to change the strategy:

- `AutoAsync`: default option, automatically flushes data in an asynchronous way.
- `AutoSameThread`: automatically flushes data in the thread where the buffer was filled in a synchronous way.
- `ManualAsync`: flushes data after adding the log in an asynchronous way
- `ManualSameThread`: flushes data after adding the log in the thread where the buffer was filled in a synchronous way.

Furthermore, loggers can be manually flushed by calliing the `flush()` function without needing add a message.

> :warning: `localog` only handle automatic or manual flushes.

**3. Log priority:** `ColloLog` offers different priority levels. When a higher priority is set, lower leveled logs will be completely ignored. The default value is `debug`.

Here is the list of priorities with the logging method associated with it in from most to least prioritized:

- `crit` called with `crit()`

- `warn` called with `warn()`

- `info` called with `info()`

- `debug` called with `debug()`

Here is an example of logging with different priority levels.

```c
void foo() {
    collog::setLevel(LogLevel::Critical)
    collog::info("non-critical message");   // won't be saved
    collog::crit("must save this!");        // will be saved
    // ...
    collog::setLevel(LogLevel::Info);
    collog::addInfo("non-critical message");   // will be saved
}
```

> :warning: For `LocalLogger`, the priority level is relative to its thread. In other words, `LocalLogger`priority level is the same for one specific thread but can change between threads.

---

## How to run benchmarks

Change the paths in `benchmark/src/main.cpp` and change the logger in `runLongBenchmark()` inside `benchmark/src/benchmarks.cpp` as you want.

> *More flexibility will be added later. This clearly not an ideal setup*. 

Before running benchmarks, close unnecessary programs, disable power saving and remove temporary files.

Then, use thoses commands:

```bash
cd ../benchmark
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=../build/install
cmake --build build --config Release
build/Release/ColloLogBench.exe
```

---

## Contributions

At this time, ColloLog is not accepting external contributions, but feel free to publish issues! :)
