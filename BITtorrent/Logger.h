#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <windows.h>

// Уровни логирования
enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	LOG_ERROR,
	CRITICAL
};

// Структура для записи логов
struct LogEntry {
	LogLevel level;
	std::string message;
	std::string timestamp;
	std::string source;
};

// Класс для логирования
class Logger {
public:
	Logger();
	~Logger();

	static Logger& GetInstance();

	void Log(LogLevel level, const std::string& message, const std::string& source = "System");
	void Debug(const std::string& message, const std::string& source = "System");
	void Info(const std::string& message, const std::string& source = "System");
	void Warning(const std::string& message, const std::string& source = "System");
	void Error(const std::string& message, const std::string& source = "System");
	void Critical(const std::string& message, const std::string& source = "System");

	std::vector<LogEntry> GetLogs(LogLevel minLevel = LogLevel::DEBUG) const;
	void ClearLogs();
	bool SaveLogsToFile(const std::string& filepath);

private:
	std::vector<LogEntry> logs;
	static const size_t MAX_LOGS = 10000;

	std::string GetLevelString(LogLevel level);
	std::string GetCurrentTimestamp();
};

// Класс для работы с окном логов
class LogWindow {
public:
	LogWindow(HWND parentWindow);
	~LogWindow();

	bool Create();
	bool Show();
	bool Hide();
	void Destroy();
	void AddLog(const LogEntry& entry);
	void ClearLogs();
	void UpdateDisplay();

private:
	HWND parentWindow;
	HWND logWindowHandle;
	HWND logListHandle;

	std::vector<LogEntry> displayedLogs;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LogWindow* g_pThis;

	void OnCreate();
	void OnClose();
};
