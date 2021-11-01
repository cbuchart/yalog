# yalog

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/367476e06a8644029452da48355d8713)](https://app.codacy.com/gh/cbuchart/yalog?utm_source=github.com&utm_medium=referral&utm_content=cbuchart/yalog&utm_campaign=Badge_Grade_Settings)

Yet Another Log library in C++, based on Boost.log.

## Compilation

- Clone or download the repository
- `mkdir build && cd build && cmake ..`

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
