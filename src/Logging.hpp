#pragma once

#define LOG(LEVEL) ::Logging::Log(Logging::LEVEL)

#include <ostream>

class Logging
{
public:
    enum Level
    {
        Error = 0,
        Warning,
        Info,
        Debug,

        Max,

        // For nicer preprocessor look
        ERROR = Error,
        WARN = Warning,
        INFO = Info,
        DEBUG = Debug
    };

    static void SetLevel(Level aLogLevel);
    static std::ostream& Log(Level aLogLevel);

private:
    static Level s_logLevel;
};
