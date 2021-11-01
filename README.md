# yalog

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/367476e06a8644029452da48355d8713)](https://app.codacy.com/gh/cbuchart/yalog?utm_source=github.com&utm_medium=referral&utm_content=cbuchart/yalog&utm_campaign=Badge_Grade_Settings)

Yet Another Log library in C++, based on Boost.log.

## Compilation

```console
$ git clone https://github.com/cbuchart/yalog.git
$ cd yalog && mkdir build && cd build
$ cmake .. && make -j
```

## Basic usage

### Initialization

- Setup log to file under `logs` directory, with logs no larger than 1MB and no more than 100 logs: `yalog::log_to_file("logs", "log", 1024 * 1024, 100);`
- Setup log to console (standard output): `yalog::log_to_stream(std::cout);`
- Set minimum severity level to `Debug` (`Warning` by default): `yalog::set_minimum_severity(yalog::Severity::Debug);`

### Available macros

Log macros for severity level:
- `YALOG_TRACE()`
- `YALOG_DEBUG()`
- `YALOG_INFO()`
- `YALOG_WARNING()`
- `YALOG_ERROR()`
- `YALOG_FATAL()`

Example: `YALOG_DEBUG() << "This is a sample message";`

Other useful macros
- `YALOG_INOUT()`: adds a log record (trace) indicating function name and when reaching the end of the current scope (useful for tracing functions).
- `YALOG_LINE()`: adds a log record (trace) indicating current filename and line number.
- `YALOG_EXPRESSION(expr)`: adds a log record (debug) for the given expression and its value.

Please check the given example for more information.

### Example

```cpp
#include <iostream>
#include <string>

#include "yalog.h"

void foo()
{
    YALOG_INOUT();

    YALOG_LINE();
    YALOG_EXPRESSION(40 + 2);
}

int main()
{
    yalog::log_to_file("logs", "log", 128 * 1024, 32u);
    yalog::log_to_stream(std::cout);

    YALOG_INFO() << "This should not appear";
    yalog::set_min_severity(yalog::Severity::Trace);

    YALOG_TRACE() << "Test for trace, string " << "Hello world!";
    YALOG_DEBUG() << "Test for debug, double " << 3.141592;
    YALOG_INFO() << "Test for info, uint32_t " << 42u;
    YALOG_WARNING() << "Test for warning";
    YALOG_ERROR() << "Test for error";
    YALOG_FATAL() << "Test for fatal";

    foo();

    return 0;
}
```

![example_screenshot](https://user-images.githubusercontent.com/5821978/139754899-a949079f-df3d-4d66-9a8f-c25fd6d34cd9.png)

