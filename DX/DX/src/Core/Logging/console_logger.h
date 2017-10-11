#pragma once

/**********************************************************************
    class: ConsoleLogger (console_logger.h)

    author: S. Hau
    date: October 10, 2017

    Features:
     - Basic logging functionality to the console

**********************************************************************/

#include "i_logger.hpp"
#include "Console/console.h"

namespace Core {  namespace Logging  {


    //**********************************************************************
    // Basic logging functionality to the console
    //**********************************************************************
    class ConsoleLogger : public ILogger
    {
        const Color defaultColor = Color::WHITE;

    public:
        ConsoleLogger() {}

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // ILogger Interface
        //----------------------------------------------------------------------
        virtual void _Log(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel, Color color) const override;
        virtual void _Log(LOGCHANNEL channel, const char* msg, Color color) const override;

        virtual void _Warn(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const override;
        virtual void _Error(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const override;

    private:
        Console m_console;

        ConsoleLogger(const ConsoleLogger& other)               = delete;
        ConsoleLogger& operator = (const ConsoleLogger& other)  = delete;
        ConsoleLogger(ConsoleLogger&& other)                    = delete;
        ConsoleLogger& operator = (ConsoleLogger&& other)       = delete;
    };

 } }