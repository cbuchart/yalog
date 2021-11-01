/*
MIT License

Copyright (c) 2021 Carlos Buchart

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <iostream>
#include <string>

#include "yalog.h"

using namespace std::string_literals;

// Ease the comutation of size in bytes
using memory_size_t = uint64_t;
memory_size_t operator""_b(uint64_t bytes)
{
    return bytes;
}
memory_size_t operator""_kb(uint64_t kilobytes)
{
    return kilobytes * 1024;
}
memory_size_t operator""_mb(uint64_t megabytes)
{
    return megabytes * 1024 * 1024;
}

void foo()
{
    YALOG_INOUT();

    YALOG_LINE();
    YALOG_EXPRESSION(40 + 2);
}

int main()
{
    yalog::log_to_file("logs", "log", 128_kb, 32u);
    yalog::log_to_stream(std::cout);

    YALOG_INFO() << "This should not appear";
    yalog::set_min_severity(yalog::severity_t::trace);

    YALOG_TRACE() << "Test for trace, std::string "
                  << "Hello world!"s;
    YALOG_DEBUG() << "Test for debug, double " << 3.141592;
    YALOG_INFO() << "Test for info, uint32_t " << 42u;
    YALOG_WARNING() << "Test for warning";
    YALOG_ERROR() << "Test for error";
    YALOG_FATAL() << "Test for fatal";

    foo();

    return 0;
}
