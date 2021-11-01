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
#ifndef YALOG_H
#define YALOG_H

#include <sstream>
#include <string>

#include <boost/log/trivial.hpp>

// The core of the log system
namespace yalog
{
    using severity_t = boost::log::trivial::severity_level;

    // Configure log to be saved to a file
    void log_to_file(std::string const& dir, std::string const& prefix, size_t max_file_size, uint32_t max_file_count);

    // Configure log to be printed on a stream (like std::cout)
    void log_to_stream(std::ostream& os);

    // Print a message to log
    void log(severity_t severity, std::string msg);

    // Sets the minimum severity to be logged, messages with a lower severity level
    // will be discarded
    void set_min_severity(severity_t severity);

    severity_t get_min_severity();

    namespace details
    {
        // Helper streamer to ease the use of log with multiple data types by composing
        // an intermediate buffer with the message and flushing to log on close
        struct Stream
        {
            bool enabled;
            severity_t severity;
            std::stringstream sstr;

            explicit Stream(severity_t severity) : severity{severity} { enabled = get_min_severity() <= severity; }

            ~Stream()
            {
                // Actually print to log on close
                if (enabled) { log(severity, sstr.str()); }
            }

            // Add value to temporary string stream
            template<typename T>
            Stream& operator<<(T&& value)
            {
                if (enabled) { sstr << value; }
                return *this;
            }
        };

        struct InOut
        {
            const std::string name;

            explicit InOut(std::string a_name) : name{std::move(a_name)} { Stream{severity_t::trace} << name << " {"; }
            ~InOut() { Stream{severity_t::trace} << "} " << name; }
        };
    } // namespace details

    // Helper functions to log messages depending on severity level
    inline auto trace() { return details::Stream{boost::log::trivial::trace}; }
    inline auto debug() { return details::Stream{boost::log::trivial::debug}; }
    inline auto info() { return details::Stream{boost::log::trivial::info}; }
    inline auto warning() { return details::Stream{boost::log::trivial::warning}; }
    inline auto error() { return details::Stream{boost::log::trivial::error}; }
    inline auto fatal() { return details::Stream{boost::log::trivial::fatal}; }
    inline auto in_out(std::string name) { return details::InOut{std::move(name)}; }
}; // namespace yalog

// Helper macros
#define YALOG_TRACE() yalog::trace()
#define YALOG_DEBUG() yalog::debug()
#define YALOG_INFO() yalog::info()
#define YALOG_WARNING() yalog::warning()
#define YALOG_ERROR() yalog::error()
#define YALOG_FATAL() yalog::fatal()
#define YALOG_HELPER_JOIN2(x, y) x##y
#define YALOG_HELPER_JOIN(x, y) YALOG_HELPER_JOIN2(x, y)
#define YALOG_INOUT() auto YALOG_HELPER_JOIN(log_inout_, __LINE__) = yalog::in_out(__FUNCTION__)
#define YALOG_LINE() YALOG_TRACE() << __FILE__ << "@" << __LINE__
#define YALOG_EXPRESSION(expr) YALOG_DEBUG() << #expr << " = " << (expr);

#endif
