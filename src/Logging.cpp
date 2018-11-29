#include "Logging.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

Logging::Level Logging::s_logLevel = Logging::Info;
namespace
{
    class NullBuffer : public std::streambuf
    {
    public:
      int overflow(int c) override { return c; }
    };

    NullBuffer null_buffer;
    std::ostream null_stream(&null_buffer);
}

char LogLevelCode[] = "EWID";

void Logging::SetLevel(Level aLogLevel)
{
    s_logLevel = aLogLevel;
}
std::ostream& Logging::Log(Level aLogLevel)
{
    if (aLogLevel <= s_logLevel)
    {
        std::time_t now_c = std::time(nullptr);
        std::cerr << std::put_time(std::localtime(&now_c), "%H:%M:%S") << "|" << LogLevelCode[aLogLevel] << " ";
        return std::cerr;
    }

    return null_stream;
}
