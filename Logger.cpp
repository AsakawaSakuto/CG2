#include "Logger.h"
#include <Windows.h>

namespace {
    std::ofstream logFile;
}

void Logger::Initialize() {
    logFile.open("Log.txt", std::ios::out);
    Log("=== Logger Initialized ===");
}

void Logger::Log(const std::string& message) {
    // コンソール出力（白）
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    std::cout << "[Log] " << message << std::endl;

    // ファイル出力
    if (logFile.is_open()) {
        logFile << "[Log] " << message << std::endl;
    }
}

void Logger::Error(const std::string& message) {
    // コンソール出力（赤）
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    std::cerr << "[Error] " << message << std::endl;

    // ファイル出力
    if (logFile.is_open()) {
        logFile << "[Error] " << message << std::endl;
    }
}

void Logger::Finalize() {
    Log("=== Logger Finalized ===");
    if (logFile.is_open()) {
        logFile.close();
    }
}