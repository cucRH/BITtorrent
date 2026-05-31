#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <map>

// Метрики производительности приложения
struct PerformanceMetrics {
	float cpuUsagePercent;
	uint64_t memoryUsageMB;
	uint64_t maxMemoryMB;
	uint32_t activeThreads;
	float averageLatency;
	uint64_t totalErrors;
	uint64_t totalWarnings;
};

// Класс для мониторинга производительности
class PerformanceMonitor {
public:
	PerformanceMonitor();
	~PerformanceMonitor();

	void StartMonitoring();
	void StopMonitoring();

	void RecordCPUUsage(float percent);
	void RecordMemoryUsage(uint64_t mb);
	void RecordThreadCount(uint32_t count);
	void RecordLatency(float ms);
	void RecordError();
	void RecordWarning();

	PerformanceMetrics GetMetrics() const;
	PerformanceMetrics GetAverageMetrics() const;
	PerformanceMetrics GetPeakMetrics() const;

	std::vector<float> GetCPUHistory() const;
	std::vector<uint64_t> GetMemoryHistory() const;

	void ResetMetrics();

private:
	PerformanceMetrics current;
	PerformanceMetrics average;
	PerformanceMetrics peak;

	std::vector<float> cpuHistory;
	std::vector<uint64_t> memoryHistory;
	std::vector<float> latencyHistory;

	bool isMonitoring;
	const size_t HISTORY_SIZE = 300;  // 5 minutes at 1 update per second

	void UpdateAverages();
};

// Класс для сбора и анализа системных событий
class SystemEventCollector {
public:
	enum class EventSeverity {
		LOW,
		MEDIUM,
		HIGH,
		CRITICAL
	};

	struct SystemEvent {
		std::string eventName;
		EventSeverity severity;
		std::string description;
		std::chrono::system_clock::time_point timestamp;
		std::string source;
	};

	SystemEventCollector();
	~SystemEventCollector();

	void RecordEvent(const SystemEvent& event);
	std::vector<SystemEvent> GetEvents(EventSeverity minSeverity = EventSeverity::LOW) const;
	std::vector<SystemEvent> GetEventsBySource(const std::string& source) const;

	size_t GetEventCount() const;
	size_t GetEventCountBySeverity(EventSeverity severity) const;

	void ClearEvents();
	void ExportEvents(const std::string& filepath);

private:
	std::vector<SystemEvent> events;
	const size_t MAX_EVENTS = 1000;
};

// Класс для анализа здоровья приложения
class ApplicationHealthMonitor {
public:
	enum class HealthStatus {
		EXCELLENT,   // Все хорошо
		GOOD,        // Незначительные проблемы
		FAIR,        // Есть проблемы
		POOR,        // Серьезные проблемы
		CRITICAL     // Критическое состояние
	};

	ApplicationHealthMonitor();
	~ApplicationHealthMonitor();

	void Update();

	HealthStatus GetHealthStatus() const;
	float GetHealthScore() const;  // 0-100

	std::string GetHealthReport() const;
	std::vector<std::string> GetIssues() const;
	std::vector<std::string> GetRecommendations() const;

	bool IsHealthy() const;

private:
	PerformanceMonitor performanceMonitor;
	SystemEventCollector eventCollector;

	HealthStatus currentStatus;
	float healthScore;
	std::vector<std::string> currentIssues;
	std::vector<std::string> recommendations;

	void AnalyzeHealth();
	void IdentifyIssues();
	void GenerateRecommendations();
};
