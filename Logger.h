#pragma once
#include <thread>
#include "MsgQueue.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <filesystem>
#include <source_location>
#include <cstring>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

std::string Level_to_string(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:    return "I";
    case LogLevel::WARNING: return "W";
    case LogLevel::ERROR:   return "E";
    case LogLevel::DEBUG:   return "D";
    default:                return "U";
    }
}

struct LogEntry {
    char* message;

    LogEntry() : message(nullptr) {}

    LogEntry(const char* msg)
    {
        size_t len = std::strlen(msg) + 1;
        message = new char[len];

        strncpy_s(message, len, msg, _TRUNCATE);
    }

    ~LogEntry()
    {
        delete[] message;
    }

    LogEntry(const LogEntry& other)
    {
        size_t len = std::strlen(other.message) + 1;
        message = new char[len];
        strncpy_s(message, len, other.message, _TRUNCATE);
    }

    LogEntry& operator=(const LogEntry& other)
    {
        if (this != &other)
        {
            delete[] message;
            size_t len = std::strlen(other.message) + 1;
            message = new char[len];
            strncpy_s(message, len, other.message, _TRUNCATE);
        }
        return *this;
    }

    LogEntry(LogEntry&& other) noexcept : message(other.message)
    {
        other.message = nullptr;
    }

    LogEntry& operator=(LogEntry&& other) noexcept
    {
        if (this != &other) {
            delete[] message;
            message = other.message;
            other.message = nullptr;
        }
        return *this;
    }
};



class Logger {
public:
    Logger(const std::string& filePath) : logFilePath(filePath), stopLogging(false) 
    {
        std::filesystem::path logPath = filePath;
        if (!std::filesystem::exists(logPath.parent_path())) 
        {
            std::filesystem::create_directories(logPath.parent_path());
        }

        logFile.open(filePath, std::ios::app); 

        if (!logFile.is_open()) 
        {
            throw std::runtime_error("Cannot open log file: " + filePath);
        }

        loggingThread = std::thread(&Logger::processEntries, this);
    }

    ~Logger() 
    {
        stopLogging = true;
        entries.push(LogEntry{}); // Push an empty message to unblock the logging thread

        if (logFile.is_open()) 
        {
            logFile.close();
        }
        if (loggingThread.joinable()) 
        {
            loggingThread.join();
        }
    }

    void log(const char* message, LogLevel level) 
    {
        const std::source_location location = std::source_location::current();

        auto now = std::chrono::system_clock::now();
        auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
        auto zonedTime = std::chrono::zoned_time{ std::chrono::current_zone(), nowSec };
        auto timeStr = std::format("{:%Y-%m-%d %H:%M:%S}", zonedTime);

        std::ostringstream logStream;
        logStream << timeStr;
        logStream << " [" << Level_to_string(level) << "]";
        logStream << " [" << location.file_name() << ":" << location.function_name() << ":" << location.line() << "]";
        logStream << " - " << message;

        auto msg = logStream.str();
        entries.push(LogEntry(msg.c_str()));
    }

    void stop() {
        stopLogging = true; // Signal to stop
        if (loggingThread.joinable()) {
            loggingThread.join(); // Wait for the logging thread to finish
        }
        logFile.close();
    }

private:

    std::ofstream logFile;
    std::string logFilePath;
    ThreadSafeQueue<LogEntry> entries;
    std::thread loggingThread;
    std::atomic<bool> stopLogging;

    void processEntries() 
    {
        LogEntry entry;
        while (!stopLogging) 
        {
            if (entries.try_pop(entry))
            {
                logFile << entry.message << std::endl;
            }
            else 
            {
                std::this_thread::yield();
            }
        }
    }
};
