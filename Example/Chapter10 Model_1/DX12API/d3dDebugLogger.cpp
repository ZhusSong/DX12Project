#include "d3dDebugLogger.h"
#include <cstdlib>
#include <ctime>
#include <Windows.h>
std::ofstream DebugLog::m_error_log_file;
std::ofstream DebugLog::m_info_log_file;
std::ofstream DebugLog::m_warn_log_file;

void initLogger(const std::string& info_log_filename,
    const std::string& warn_log_filename,
    const std::string& error_log_filename) 
{
    DebugLog::m_info_log_file.open(info_log_filename.c_str());
    DebugLog::m_warn_log_file.open(warn_log_filename.c_str());
    DebugLog::m_error_log_file.open(error_log_filename.c_str());
}

std::ostream& DebugLog::getStream(logRank log_rank,bool print_to_console) 
{
    if (print_to_console) {
        return (Info == log_rank) ? std::cout : std::cerr;
    }
    else {
        return (Info == log_rank) ?
            (m_info_log_file.is_open() ? m_info_log_file : std::cout) :
            (Warning == log_rank ?
                (m_warn_log_file.is_open() ? m_warn_log_file : std::cerr) :
                (m_error_log_file.is_open() ? m_error_log_file : std::cerr));
    }
}
std::ostream& DebugLog::start(logRank log_rank,
    const int line,
    const std::string& function) 
{
    time_t tm;
    time(&tm);
    std::string logMessage = ctime(&tm) +
        std::string("function (") + function + ") " +
        std::string("line: ") + std::to_string(line) + '\t';
    // 打印信息
    getStream(log_rank, true) << logMessage << std::flush;
    // 向文件传输信息
    return getStream(log_rank, false) << logMessage << std::flush;
}


DebugLog::~DebugLog() 
{
    getStream(m_log_rank,false) << std::endl << std::flush;

    if (FATAL == m_log_rank) {
        m_info_log_file.close();
        m_info_log_file.close();
        m_info_log_file.close();
        abort();
    }
}