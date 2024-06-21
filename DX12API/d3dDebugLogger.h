#pragma once

#ifndef D3DDEBUGLOGGER_H
#define D3DDEBUGLOGGER_H

#include <iostream>
#include <iomanip>
#include <fstream>  
#include <string>  
#include <cstdlib>
#include <stdint.h>

typedef enum LogType {
    Info = 0,
    Warning = 1,
    Error = 2,
    FATAL=3
}logRank;

// 初始化日志文件
// info_log_filename 信息文件的名字
// warn_log_filename 警告文件的名字
// error_log_filename 错误文件的名字
void initLogger(const std::string& info_log_filename,
    const std::string& warn_log_filename,
    const std::string& error_log_filename);

class DebugLog
{
    friend void initLogger(const std::string& info_log_filename,
        const std::string& warn_log_filename,
        const std::string& erro_log_filename);

public:
    DebugLog(logRank log_rank) :m_log_rank(log_rank) {};
    ~DebugLog();

    static std::ostream& start(logRank log_rank,
        const int line,
        const std::string& function);


private:
    // 根据等级获取相应的日志输出流
    static std::ostream& getStream(logRank log_rank,bool printf_to_console);

    static std::ofstream m_info_log_file;                   ///< 信息日子的输出流
    static std::ofstream m_warn_log_file;                  ///< 警告信息的输出流
    static std::ofstream m_error_log_file;                  ///< 错误信息的输出流
    logRank m_log_rank;                             ///< 日志的信息的等级
};

// 根据不同等级进行用不同的输出流进行读写
#define LOG(log_rank)   \
DebugLog(log_rank).start(log_rank, __LINE__,__FUNCTION__)

#define Debug(log_rank, message) \
    do { \
        std::ostream& output = (Info == log_rank) ? std::cout : std::cerr; \
        output << message << std::endl; \
    } while (0)

// 利用日志进行检查的各种宏
#define CHECK(a)                                            \
   if(!(a)) {                                              \
       LOG(ERROR) << " CHECK failed " << endl              \
                   << #a << "= " << (a) << endl;          \
       abort();                                            \
   }                                                      \

#define CHECK_NOTNULL(a)                                    \
   if( NULL == (a)) {                                      \
       LOG(ERROR) << " CHECK_NOTNULL failed "              \
                   << #a << "== NULL " << endl;           \
       abort();                                            \
    }

#define CHECK_NULL(a)                                       \
   if( NULL != (a)) {                                      \
       LOG(ERROR) << " CHECK_NULL failed " << endl         \
                   << #a << "!= NULL " << endl;           \
       abort();                                            \
    }


#define CHECK_EQ(a, b)                                      \
   if(!((a) == (b))) {                                     \
       LOG(ERROR) << " CHECK_EQ failed "  << endl          \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_NE(a, b)                                      \
   if(!((a) != (b))) {                                     \
       LOG(ERROR) << " CHECK_NE failed " << endl           \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_LT(a, b)                                      \
   if(!((a) < (b))) {                                      \
       LOG(ERROR) << " CHECK_LT failed "                   \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_GT(a, b)                                      \
   if(!((a) > (b))) {                                      \
       LOG(ERROR) << " CHECK_GT failed "  << endl          \
                  << #a <<" = " << (a) << endl            \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_LE(a, b)                                      \
   if(!((a) <= (b))) {                                     \
       LOG(ERROR) << " CHECK_LE failed "  << endl          \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_GE(a, b)                                      \
   if(!((a) >= (b))) {                                     \
       LOG(ERROR) << " CHECK_GE failed "  << endl          \
                   << #a << " = "<< (a) << endl            \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_DOUBLE_EQ(a, b)                               \
   do {                                                    \
       CHECK_LE((a), (b)+0.000000000000001L);              \
       CHECK_GE((a), (b)-0.000000000000001L);              \
    }while (0)

#endif

