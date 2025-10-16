# ColloLog

ColloLog is a lightweight, high-performance C++ logging library designed for multithreaded and performance-critical applications. It offers efficient, thread-safe logging with minimal performance impact.

![License: MIT](https://img.shields.io/badge/license-MIT-blue)
![Platform: Windows](https://img.shields.io/badge/platform-Windows-blue)
![Status: In Development](https://img.shields.io/badge/status-in_development-yellow)

---

## Why use ColloLog?

`ColloLog` is designed for general-purpose and performance logging in multithreaded environments. It offers:

- Plain text output (for readability)

- Timestamps (program's runtime timer for efficiency)

- Static format (for optimization)

**For single-threaded projects**, ``ColloLog`` is still highly performant by offering a throughput of **11'5000'000 logs/s** which is the fastest plain-text logger I know of.

### So when to use this tool?

- High-performance systems

- Multithreaded or concurrent systems

- Benchmarking

- Low disk foot-print requirements

## Adapting to your project

`ColloLog` provides two logger variants, optimized for different threading models:

- `ColloLogger`: highly performant for **few threads** (1-3)

- `LocalLogger`: highly performant for **multiple threads** (4+)

**Throughput example:** Logs were automatically flushed to disk by the tool and messages were static.

- [1 thread] `ColloLogger`: 15'000'000 logs/s

- [2 threads] `ColloLogger`: 14'800'000 logs/s

- [8 threads] `LocalLogger`: 11'000'000 logs/s

> :exclamation: Benchmarks were run on an Intel i7-13620H (13th gen), 32GB RAM.

---

## About Collineo

Collineo Inc. is committed to delivering top-quality solutions to its clients. With expertise in automation machinery and software development, Collineo serves a diverse range of industries, leveraging adaptability as one of its greatest strengths.

Link to our [website][1] and to our [LinkedIn][2]

[1]: http://www.collineo.net
[2]: https://ca.linkedin.com/company/collineo-inc

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

```bash
mkdir ColloLog/build;
cd ColloLog/build;
cmake .. -DCMAKE_BUILD_TYPE=Release;
cmake --build . --config Release;
cmake --install . --prefix install --config Release;
cmake .. -DCMAKE_BUILD_TYPE=Debug;
cmake --build . --config Debug;
cmake --install . --prefix install --config Debug;
```

- Link it with your build setup

## Integrating to your projects

> :warning: `ColloLog` uses its own implementation of thread pools. To allow your current thread pools to work with `ColloLog`, a thread pool interface will be added.

> :warning: These steps might change before going through version 1. 

1. In main.cpp, instantiate the thread pool with the number of threads that is best for the project.

```c
#include <ColloLog/ThreadPool.h>

constexpr unsigned int NUMBER_OF_THREADS = 3;

ThreadPool pool(NUMBER_OF_THREADS);
```

2. Instantiate a logger globally in the main.cpp after the thread pool.

```c
// ...
ThreadPool pool(NUMBER_OF_THREADS);
ColloLogger logger("path/to/logs");
```

3. Optionally, multiple loggers can be created if some systems need their own sink.

```c
// ...
ColloLogger logger("path/to/logs");
ColloLogger userLogger("path/to/user/logs");
```

4. `ColloLog` offers different priority levels. When a higher priority is set, lower leveled logs will be completely ignored. The default value is `debug`.

Here is the list of priorities with the logging method associated with it in from most to least prioritized:

- `crit` called with `addCrit()`

- `warn` called with `addWarn()`

- `info` called with `addInfo()`

- `debug` called with `addDebug()`

Here is an example of logging with different priority levels.

```c
#include <ColloLog/Levels.h>

void foo() {
    logger.setLevel(LogLevel::critical)
    logger.addInfo("non-critical message");   // won't be saved
    logger.addCrit("must save this!");        // will be saved
    // ...
    logger.setLevel(LogLevel::info);
    logger.addInfo("non-critical message");   // will be saved
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
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=../install
cmake --build build --config Release
build/Release/ColloLogBench.exe
```

---

## Contributions

At this time, ColloLog is not accepting external contributions, but feel free to publish issues!
