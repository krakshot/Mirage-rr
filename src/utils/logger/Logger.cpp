#include "Logger.hpp"
#include <iomanip>
#include <chrono>
#include <sstream>
#include <fstream>
#include <deque>
#include <mutex>
#include <thread>
#include <imgui.h>

static std::deque<LogEntry> s_entries;
static std::mutex s_entries_mutex;
static std::mutex s_init_mutex;
static bool s_initialized = false;
static const size_t MAX_ENTRIES = 2000;
static const char* LOG_FILE = "mirage_logs.txt";

std::string Logger::GetTime()
{
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

    std::tm tmLocal{};
    localtime_s(&tmLocal, &t);

    std::stringstream ss;
    ss << std::put_time(&tmLocal, "%H:%M:%S");
    return ss.str();
}
void Logger::InitializeConsoleColors()
{

}

void Logger::PrintPrefix(char symbol)
{

}

static const char* LevelToString(LogEntry::Level l) {
    switch (l) {
    case LogEntry::Success: return "[SUCCESS]";
    case LogEntry::Error: return "[ERROR]";
    case LogEntry::Debug: return "[DEBUG]";
    case LogEntry::Prompt: return "[PROMPT]";
    default: return "[INFO]";
    }
}

void Logger::AddEntry(LogEntry::Level level, const std::string& text)
{

    {
        std::lock_guard<std::mutex> init_lock(s_init_mutex);
        if (!s_initialized) {

            std::ofstream ofs(LOG_FILE, std::ios::app);
            if (ofs) {
                auto now = GetTime();
                ofs << "[" << now << "] [DEBUG] Logger initialized" << std::endl;
            }

            {
                std::lock_guard<std::mutex> lock(s_entries_mutex);
                s_entries.push_back(LogEntry{ LogEntry::Debug, GetTime(), "Logger initialized" });
                if (s_entries.size() > MAX_ENTRIES) s_entries.pop_front();
            }
            s_initialized = true;
        }
    }

    LogEntry e{ level, GetTime(), text };

    {
        std::lock_guard<std::mutex> lock(s_entries_mutex);
        s_entries.push_back(e);
        if (s_entries.size() > MAX_ENTRIES) s_entries.pop_front();
    }


    std::ofstream ofs(LOG_FILE, std::ios::app);
    if (ofs) {
        ofs << "[" << e.time << "] " << LevelToString(e.level) << " " << e.text << std::endl;
    }
}

void Logger::Initialize()
{
    std::lock_guard<std::mutex> init_lock(s_init_mutex);
    if (s_initialized) return;

    std::ofstream ofs(LOG_FILE, std::ios::app);
    if (ofs) {
        auto now = GetTime();
        ofs << "[" << now << "] [DEBUG] Logger initialized (explicit)" << std::endl;
    }

    {
        std::lock_guard<std::mutex> lock(s_entries_mutex);
        s_entries.push_back(LogEntry{ LogEntry::Debug, GetTime(), "Logger initialized (explicit)" });
        if (s_entries.size() > MAX_ENTRIES) s_entries.pop_front();
    }
    s_initialized = true;
}

void Logger::Success(const std::string& text)
{
    AddEntry(LogEntry::Success, text);
}

void Logger::Error(const std::string& text)
{
    AddEntry(LogEntry::Error, text);
}

void Logger::Debug(const std::string& text)
{
    AddEntry(LogEntry::Debug, text);
}

void Logger::Prompt(const std::string& text)
{
    AddEntry(LogEntry::Prompt, text);
}

void Logger::InlineSuccess(const std::string& text)
{
    AddEntry(LogEntry::Success, text);
}

void Logger::Countdown(int seconds)
{
    for (int i = seconds; i >= 0; --i)
    {
        AddEntry(LogEntry::Success, std::string("Closing in ") + std::to_string(i) + " seconds");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::vector<LogEntry> Logger::GetEntries()
{
    std::lock_guard<std::mutex> lock(s_entries_mutex);
    return std::vector<LogEntry>(s_entries.begin(), s_entries.end());
}

void Logger::ClearEntries()
{
    {
        std::lock_guard<std::mutex> lock(s_entries_mutex);
        s_entries.clear();
    }

    std::ofstream ofs(LOG_FILE, std::ios::trunc);
}

void Logger::Render()
{
    auto entries = GetEntries();

    ImVec2 sz = ImGui::GetContentRegionAvail();
    float child_h = sz.y - 22.0f;
    if (child_h < 10.0f) child_h = 10.0f;
    if (sz.x < 10.0f) sz.x = 10.0f;
    ImGui::BeginChild("##logger_internal_child", ImVec2(sz.x, child_h), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < entries.size(); ++i)
    {
        const LogEntry& e = entries[i];
        ImU32 col = IM_COL32(200,200,200,255);
        if (e.level == LogEntry::Error) col = IM_COL32(220, 100, 100, 255);
        else if (e.level == LogEntry::Success) col = IM_COL32(140, 200, 180, 255);
        else if (e.level == LogEntry::Debug) col = IM_COL32(170, 170, 240, 255);
        else if (e.level == LogEntry::Prompt) col = IM_COL32(200, 180, 220, 255);

        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextWrapped("[%s] %s", e.time.c_str(), e.text.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();
}