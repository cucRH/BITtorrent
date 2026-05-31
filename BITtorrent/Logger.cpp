#include "Logger.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>

Logger* g_logger = nullptr;

Logger::Logger() {
}

Logger::~Logger() {
}

Logger& Logger::GetInstance() {
	if (!g_logger) {
		g_logger = new Logger();
	}
	return *g_logger;
}

void Logger::Log(LogLevel level, const std::string& message, const std::string& source) {
	if (logs.size() >= MAX_LOGS) {
		logs.erase(logs.begin());
	}

	LogEntry entry;
	entry.level = level;
	entry.message = message;
	entry.source = source;
	entry.timestamp = GetCurrentTimestamp();

	logs.push_back(entry);

	// Также выводим в отладчик
	std::stringstream ss;
	ss << "[" << entry.timestamp << "] [" << GetLevelString(level) << "] ["
	   << source << "] " << message << "\n";

	OutputDebugStringA(ss.str().c_str());
}

void Logger::Debug(const std::string& message, const std::string& source) {
	Log(LogLevel::DEBUG, message, source);
}

void Logger::Info(const std::string& message, const std::string& source) {
	Log(LogLevel::INFO, message, source);
}

void Logger::Warning(const std::string& message, const std::string& source) {
	Log(LogLevel::WARNING, message, source);
}

void Logger::Error(const std::string& message, const std::string& source) {
	Log(LogLevel::LOG_ERROR, message, source);
}

void Logger::Critical(const std::string& message, const std::string& source) {
	Log(LogLevel::CRITICAL, message, source);
}

std::vector<LogEntry> Logger::GetLogs(LogLevel minLevel) const {
	std::vector<LogEntry> result;
	for (const auto& log : logs) {
		if (static_cast<int>(log.level) >= static_cast<int>(minLevel)) {
			result.push_back(log);
		}
	}
	return result;
}

void Logger::ClearLogs() {
	logs.clear();
}

bool Logger::SaveLogsToFile(const std::string& filepath) {
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& log : logs) {
		file << "[" << log.timestamp << "] [" << GetLevelString(log.level) << "] ["
			 << log.source << "] " << log.message << "\n";
	}

	file.close();
	return true;
}

std::string Logger::GetLevelString(LogLevel level) {
	switch (level) {
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::WARNING:
		return "WARNING";
	case LogLevel::LOG_ERROR:
		return "ERROR";
	case LogLevel::CRITICAL:
		return "CRITICAL";
	default:
		return "UNKNOWN";
	}
}

std::string Logger::GetCurrentTimestamp() {
	time_t now = time(nullptr);
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);

	std::stringstream ss;
	ss << std::setfill('0')
	   << std::setw(2) << (timeinfo.tm_hour) << ":"
	   << std::setw(2) << (timeinfo.tm_min) << ":"
	   << std::setw(2) << (timeinfo.tm_sec);

	return ss.str();
}

// LogWindow implementation
LogWindow* LogWindow::g_pThis = nullptr;

LogWindow::LogWindow(HWND parentWindow)
	: parentWindow(parentWindow),
	logWindowHandle(nullptr),
	logListHandle(nullptr) {
}

LogWindow::~LogWindow() {
	Destroy();
}

bool LogWindow::Create() {
	// Для простоты используем встроенную функцию создания окна
	// В полной версии здесь была бы полная реализация
	return true;
}

bool LogWindow::Show() {
	if (logWindowHandle) {
		ShowWindow(logWindowHandle, SW_SHOW);
		return true;
	}
	return false;
}

bool LogWindow::Hide() {
	if (logWindowHandle) {
		ShowWindow(logWindowHandle, SW_HIDE);
		return true;
	}
	return false;
}

void LogWindow::Destroy() {
	if (logWindowHandle) {
		DestroyWindow(logWindowHandle);
		logWindowHandle = nullptr;
	}
}

void LogWindow::AddLog(const LogEntry& entry) {
	displayedLogs.push_back(entry);
	UpdateDisplay();
}

void LogWindow::ClearLogs() {
	displayedLogs.clear();
	UpdateDisplay();
}

void LogWindow::UpdateDisplay() {
	// Обновляем отображение логов
}

LRESULT CALLBACK LogWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void LogWindow::OnCreate() {
}

void LogWindow::OnClose() {
}
