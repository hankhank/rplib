#ifndef RPDEBUG_H_
#define RPDEBUG_H_

/**
 * Alot of inspiration taken from glog
 */

#include <time.h>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include "rpsemaphore.h"

#if RP_STRIP_LOG == 0
#define COMPACT_REPOST_LOG_DEBUG LogMessage( \
      __FILE__, __FUNCTION__,  __LINE__, DEBUG)
#else
#define COMPACT_REPOST_LOG_DEBUG NullStream()
#endif

#if RP_STRIP_LOG <= 1
#define COMPACT_REPOST_LOG_INFO LogMessage( \
      __FILE__, __FUNCTION__,  __LINE__, INFO)
#else
#define COMPACT_REPOST_LOG_INFO NullStream()
#endif

#if RP_STRIP_LOG <= 2
#define COMPACT_REPOST_LOG_WARNING LogMessage( \
      __FILE__, __FUNCTION__, __LINE__, WARNING)
#else
#define COMPACT_REPOST_LOG_WARNING NullStream()
#endif

#if RP_STRIP_LOG <= 3
#define COMPACT_REPOST_LOG_ERROR LogMessage( \
      __FILE__, __FUNCTION__, __LINE__, ERROR)
#else
#define COMPACT_REPOST_LOG_ERROR NullStream()
#endif

#if RP_STRIP_LOG <= 4
#define COMPACT_REPOST_LOG_FATAL LogMessage( \
      __FILE__, __FUNCTION__, __LINE__, FATAL)
#else
#define COMPACT_REPOST_LOG_FATAL NullStreamFatal()
#endif

#define LOG_IF(severity, condition) \
    if (condition) LOG(severity)

#define LOG(severity) COMPACT_REPOST_LOG_ ## severity.stream()

namespace rp {

    typedef int LogSeverity;
    const int DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, FATAL = 4,
        NUM_SEVERITIES = 5;

    extern const char* const LogSeverityNames[NUM_SEVERITIES];

    /*
    ** Prototype for log sink. Send it messages and it sinks em.
    */
    class LogSink {
    public:
        LogSink(){};
        virtual ~LogSink(){};
        virtual void Send(LogSeverity severity, std::string msg) = 0;
    };

    /*
    ** Stdout sink. Sends debug to stdout.
    */
    class LogStdoutSink : public LogSink {
    public:
      LogStdoutSink(){};
      ~LogStdoutSink(){};
      void Send(LogSeverity severity, std::string msg);
    };

    /*
    ** Sends debug to file for analysis later
    */
    class LogFileSink : public LogSink {
    public:
        LogFileSink(std::string base_filename);
        ~LogFileSink();
        void Send(LogSeverity severity, std::string msg);
        void Write(bool force_flush, std::string msg);

        /* Configuration options */
        void SetBasename(std::string basename);
        void SetFlushTime(int secs);
        void SetFileExtension(const char* extension);

        int LogSize();
        int MaxLogSize();

        void Flush();
        /* Flush log without grabbing locks */
        void FlushUnlocked();

     private:
        static const int kRolloverAttemptFrequency = 0x20;

        RpSemaphore* lock_;
        std::string base_filename_;
        std::string file_extension_;
        FILE* file_;
        LogSeverity severity_;
        int bytes_since_flush_;
        int logbufsecs_;
        int file_length_;
        int maxlogsize_;
        unsigned int rollover_attempt_;
        int next_flush_time_; /* cycle count at which to flush log */

        bool CreateLogfile(std::string time_pid_string);
    };

    class LogMessage {
    public:
        class LogStream : public std::ostringstream {
        public:
                LogStream(int ctr):std::ostringstream(std::ostringstream::out),
                    ctr_(ctr) 
                    {
                        self_ = this;
                    }

                int ctr() const { return ctr_; }
                void set_ctr(int ctr) { ctr_ = ctr; }
                LogStream* self() const { return self_; }

            private:
                int ctr_;  /* Counter hack (for the LOG_EVERY_X() macro) */
                LogStream *self_;  /* Consistency check hack */
        };

    public:
        /* icc 8 requires this typedef to avoid an internal compiler error. */
        typedef void (LogMessage::*SendMethod)();

        LogMessage(const char* file, const char* func, 
            int line, LogSeverity severity);
        ~LogMessage();

        /* 
        ** Flush a buffered message to the sink set in the constructor.  Always
        ** called by the destructor, it may also be called from elsewhere if
        ** needed.  Only the first call is actioned; any later ones are ignored.
        */
        void Flush();

        std::ostringstream& stream() { return *(data_->stream_); }

    private:
        void Init(const char* file, const char* func, 
            int line, LogSeverity severity);

        void SendToLog();  /* Actually dispatch to the logs */

        /* 
        ** We keep the data in a separate struct so that each instance of
        ** LogMessage uses less stack space.
        */
        struct LogMessageData {
            LogMessageData() {};

            LogStream* stream_;
            char severity_;     /* What level is this LogMessage logged at? */
            int line_;          /* line number where logging call is. */
            void (LogMessage::*send_method_)();  /* Call this in destructor to send */
            time_t timestamp_;         /* Time of creation of LogMessage */
            struct ::tm tm_time_;      /* Time of creation of LogMessage */
            const char* basename_;     /* basename of file that called LOG */
            const char* fullname_;     /* fullname of file that called LOG */
            const char* func_;         /* function name that called LOG */
            bool has_been_flushed_;    /* false => data has not been flushed */

            ~LogMessageData();
            private:
            LogMessageData(const LogMessageData&);
            void operator=(const LogMessageData&);
        };

        LogMessageData* allocated_;
        LogMessageData* data_;

        LogMessage(const LogMessage&);
        void operator=(const LogMessage&);
    };

    class NullStream : public LogMessage::LogStream {
    public:
      /*
      ** Initialize the LogStream so the messages can be written somewhere
      ** (they'll never be actually displayed). This will be needed if a
      ** NullStream& is implicitly converted to LogStream&, in which case
      ** the overloaded NullStream::operator<< will not be invoked.
      */
        NullStream() : LogMessage::LogStream(0) { }
        NullStream &stream() { return *this; }
    };

    /*
    ** Do nothing. This operator is inline, allowing the message to be
    ** compiled away. The message will not be compiled away if we do
    ** something like (flag ? LOG(INFO) : LOG(ERROR)) << message; when
    ** SKIP_LOG=WARNING. In those cases, NullStream will be implicitly
    ** converted to LogStream and the message will be computed and then
    ** quietly discarded.
    */
    template<class T>
    inline NullStream& operator<<(NullStream &str, const T &value) { return str; }

    /*
    ** Initialise and close the debug library and print out a version header
    */
    void InitLogging(void);
    void ShutdownLogging(void);
    /*
    ** Add a log sink to handle incoming logs. There are two already provided.
    ** StdoutSink & FileSink
    */
    void AddLogSink(LogSink* new_sink);
    /* 
    ** Clean up old FileSink logs from dir
    */
    void CleanUpOldLogs(std::string logdirectory);
#ifdef WIN32
    /*
    ** Open a windows console to display StdoutSink output
    */
    void OpenWinConsole();
#endif

    /*
    ** Not thread-safe for us from the top level thread only.
    ** Intention here is to allow users to turn up the level
    ** of debug
    */
    void SetRepostLogLevel(LogSeverity severity);

};

#endif
