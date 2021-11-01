#include "yalog.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/shared_ptr.hpp>

using namespace std::string_literals;

namespace logging = boost::log;
namespace expr = logging::expressions;
namespace attrs = logging::attributes;
namespace sinks = logging::sinks;
namespace keywords = logging::keywords;

using file_sink_t = sinks::synchronous_sink<sinks::text_file_backend>;

// Register some attributes to be used in the log
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_timestamp, "TimeStamp", attrs::local_clock::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_thread_id, "ThreadID", attrs::current_thread_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(attr_severity, "Severity", yalog::Severity)

namespace
{
    template<typename T>
    auto to_integral(T e)
    {
        return static_cast<std::underlying_type_t<T>>(e);
    }
}

bool operator>=(yalog::Severity a, yalog::Severity b)
{
    return to_integral(a) >= to_integral(b);
}

namespace
{
    struct Log
    {
        boost::shared_ptr<file_sink_t> sink;

        logging::sources::severity_logger<yalog::Severity> logger;

        yalog::Severity min_severity;

        Log()
        {
            set_min_severity(yalog::Severity::Warning);

            // Add the attributes we want to use
            logger.add_attribute("TimeStamp", attrs::local_clock());
            logger.add_attribute("ThreadID", attrs::current_thread_id());
        }

        void set_min_severity(yalog::Severity severity)
        {
            min_severity = severity;
            logging::core::get()->set_filter(attr_severity >= severity);
        }

        ~Log()
        {
            if (sink) {
                logging::core::get()->remove_sink(sink);
                sink.reset();
            }
        }
    } s_log;

    template<bool Console>
    void formatter(logging::record_view const& rec, logging::formatting_ostream& strm)
    {
        if (Console) {
            strm << ">> ";
        } else {
            strm << "<li><pre>";
        }

        // Severity
        static const auto s_severity_labels = std::vector<std::string>{
          "trace"s, "debug"s, "info"s, "warning"s, "error"s, "fatal"s,
        };
        static const auto s_severity_colours = std::vector<std::string>{
          "\033[37m"s, "\033[32m"s, "\033[36m"s, "\033[33m"s, "\033[31m"s, "\033[37;41m"s,
        };

        auto severity = rec[attr_severity];
        if (severity) {
            auto const severity_value = to_integral(severity.get());

            if (Console) {
                // Colorize based on severity
                if (severity_value < s_severity_colours.size()) { strm << s_severity_colours[severity_value]; }
            }

            if (severity_value < s_severity_labels.size()) {
                strm << "[" << std::left << std::setw(7) << std::setfill(' ') << s_severity_labels[severity_value]
                     << "] ";
            }
        }

        // Timestamp and thread ID
        strm << rec[attr_timestamp] << " - " << rec[attr_thread_id];

        // Message
        strm << ": " << rec[expr::smessage];

        if (Console) {
            // Reset color
            if (severity) { strm << "\033[0m"; }
        } else {
            strm << "</pre></li>";
        }
    }
} // namespace

// Configure log to be saved to a file
void yalog::log_to_file(std::string const& dir, std::string const& prefix, size_t max_file_size,
                        uint32_t max_file_count)
{
    // Setup filename (based on timestamp) and rotation (everyday, up to a max
    // file size)
    auto backend = boost::make_shared<sinks::text_file_backend>(
      keywords::file_name = dir + "/" + prefix + "_%Y%m%d_%H%M%S.html", keywords::rotation_size = max_file_size,
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0) // rotate every day
    );

    s_log.sink = boost::make_shared<file_sink_t>(backend);

    s_log.sink->locked_backend()->set_file_collector(
      sinks::file::make_collector(keywords::target = dir, keywords::max_files = max_file_count));
    s_log.sink->locked_backend()->scan_for_files(sinks::file::scan_all);

    // Setup log format (HTML in this case), and messages format
    // You could add your own stylesheet here for extended decoration
    s_log.sink->locked_backend()->auto_flush(true);
    s_log.sink->locked_backend()->set_open_handler(boost::lambda::_1 << "<html><h1>Log</h1><ul>\n");
    s_log.sink->locked_backend()->set_close_handler(boost::lambda::_1 << "</ul></html>\n");
    s_log.sink->set_formatter(&formatter<false>);

    logging::core::get()->add_sink(s_log.sink);
}

// Configure log to be printed on a stream (like std::cout)
void yalog::log_to_stream(std::ostream& os)
{
    logging::add_console_log(os, logging::keywords::format = &formatter<true>);
}

// Print a message to log
void yalog::log(Severity severity, std::string msg)
{
    // auto const severity_value = static_cast<std::underlying_type_t<Severity>>(severity);
    // BOOST_LOG_SEV(s_log.logger, (logging::trivial::severity_level)severity_value) << msg;
    BOOST_LOG_SEV(s_log.logger, severity) << msg;
}

// Sets the minimum severity to be logged, messages with a lower severity level
// will be discarded
void yalog::set_min_severity(Severity severity)
{
    s_log.set_min_severity(severity);
}

yalog::Severity yalog::get_min_severity()
{
    return s_log.min_severity;
}
