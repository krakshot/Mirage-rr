#pragma once
#include <string>
#include <vector>

struct LogEntry {
    enum Level { Success = 0, Error = 1, Debug = 2, Prompt = 3 } level;
    std::string time;
    std::string text;
};

class Logger
{
public:
    static void Success(const std::string& text);
    static void Error(const std::string& text);
    static void Debug(const std::string& text);
    static void Prompt(const std::string& text);

    static void Countdown(int seconds);
    static void InlineSuccess(const std::string& text);

    static std::vector<LogEntry> GetEntries();
    static void ClearEntries();

    static void Render();

    static void Initialize();

private:
    static std::string GetTime();
    static void PrintPrefix(char symbol);
    static void InitializeConsoleColors();
    static void AddEntry(LogEntry::Level level, const std::string& text);
};