#include "PerformanceMonitor.h"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>

PerformanceMonitor::PerformanceMonitor()
	: isMonitoring(false) {
	current = { 0, 0, 0, 0, 0, 0, 0 };
	average = { 0, 0, 0, 0, 0, 0, 0 };
	peak = { 0, 0, 0, 0, 0, 0, 0 };
}

PerformanceMonitor::~PerformanceMonitor() {
	StopMonitoring();
}

void PerformanceMonitor::StartMonitoring() {
	isMonitoring = true;
}

void PerformanceMonitor::StopMonitoring() {
	isMonitoring = false;
}

void PerformanceMonitor::RecordCPUUsage(float percent) {
	current.cpuUsagePercent = percent;

	if (cpuHistory.size() >= HISTORY_SIZE) {
		cpuHistory.erase(cpuHistory.begin());
	}
	cpuHistory.push_back(percent);

	if (percent > peak.cpuUsagePercent) {
		peak.cpuUsagePercent = percent;
	}

	UpdateAverages();
}

void PerformanceMonitor::RecordMemoryUsage(uint64_t mb) {
	current.memoryUsageMB = mb;

	if (memoryHistory.size() >= HISTORY_SIZE) {
		memoryHistory.erase(memoryHistory.begin());
	}
	memoryHistory.push_back(mb);

	if (mb > peak.memoryUsageMB) {
		peak.memoryUsageMB = mb;
	}

	UpdateAverages();
}

void PerformanceMonitor::RecordThreadCount(uint32_t count) {
	current.activeThreads = count;
}

void PerformanceMonitor::RecordLatency(float ms) {
	if (latencyHistory.size() >= HISTORY_SIZE) {
		latencyHistory.erase(latencyHistory.begin());
	}
	latencyHistory.push_back(ms);

	if (ms > peak.averageLatency) {
		peak.averageLatency = ms;
	}

	UpdateAverages();
}

void PerformanceMonitor::RecordError() {
	current.totalErrors++;
}

void PerformanceMonitor::RecordWarning() {
	current.totalWarnings++;
}

PerformanceMetrics PerformanceMonitor::GetMetrics() const {
	return current;
}

PerformanceMetrics PerformanceMonitor::GetAverageMetrics() const {
	return average;
}

PerformanceMetrics PerformanceMonitor::GetPeakMetrics() const {
	return peak;
}

std::vector<float> PerformanceMonitor::GetCPUHistory() const {
	return cpuHistory;
}

std::vector<uint64_t> PerformanceMonitor::GetMemoryHistory() const {
	return memoryHistory;
}

void PerformanceMonitor::ResetMetrics() {
	current = { 0, 0, 0, 0, 0, 0, 0 };
	average = { 0, 0, 0, 0, 0, 0, 0 };
	peak = { 0, 0, 0, 0, 0, 0, 0 };
	cpuHistory.clear();
	memoryHistory.clear();
	latencyHistory.clear();
}

void PerformanceMonitor::UpdateAverages() {
	if (!cpuHistory.empty()) {
		average.cpuUsagePercent = std::accumulate(cpuHistory.begin(), cpuHistory.end(), 0.0f) / cpuHistory.size();
	}

	if (!memoryHistory.empty()) {
		uint64_t sum = std::accumulate(memoryHistory.begin(), memoryHistory.end(), 0ULL);
		average.memoryUsageMB = sum / memoryHistory.size();
	}

	if (!latencyHistory.empty()) {
		average.averageLatency = std::accumulate(latencyHistory.begin(), latencyHistory.end(), 0.0f) / latencyHistory.size();
	}
}

// SystemEventCollector
SystemEventCollector::SystemEventCollector() {
}

SystemEventCollector::~SystemEventCollector() {
}

void SystemEventCollector::RecordEvent(const SystemEvent& event) {
	if (events.size() >= MAX_EVENTS) {
		events.erase(events.begin());
	}

	SystemEvent newEvent = event;
	newEvent.timestamp = std::chrono::system_clock::now();
	events.push_back(newEvent);
}

std::vector<SystemEventCollector::SystemEvent> SystemEventCollector::GetEvents(EventSeverity minSeverity) const {
	std::vector<SystemEvent> result;
	for (const auto& event : events) {
		if (static_cast<int>(event.severity) >= static_cast<int>(minSeverity)) {
			result.push_back(event);
		}
	}
	return result;
}

std::vector<SystemEventCollector::SystemEvent> SystemEventCollector::GetEventsBySource(const std::string& source) const {
	std::vector<SystemEvent> result;
	for (const auto& event : events) {
		if (event.source == source) {
			result.push_back(event);
		}
	}
	return result;
}

size_t SystemEventCollector::GetEventCount() const {
	return events.size();
}

size_t SystemEventCollector::GetEventCountBySeverity(EventSeverity severity) const {
	size_t count = 0;
	for (const auto& event : events) {
		if (event.severity == severity) {
			count++;
		}
	}
	return count;
}

void SystemEventCollector::ClearEvents() {
	events.clear();
}

void SystemEventCollector::ExportEvents(const std::string& filepath) {
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return;
	}

	for (const auto& event : events) {
		auto time = std::chrono::system_clock::to_time_t(event.timestamp);
		char timeStr[100];
		ctime_s(timeStr, sizeof(timeStr), &time);

		file << "Time: " << timeStr
			 << "Event: " << event.eventName << "\n"
			 << "Severity: " << (int)event.severity << "\n"
			 << "Source: " << event.source << "\n"
			 << "Description: " << event.description << "\n\n";
	}

	file.close();
}

// ApplicationHealthMonitor
ApplicationHealthMonitor::ApplicationHealthMonitor()
	: currentStatus(HealthStatus::GOOD),
	healthScore(100.0f) {
}

ApplicationHealthMonitor::~ApplicationHealthMonitor() {
}

void ApplicationHealthMonitor::Update() {
	AnalyzeHealth();
	IdentifyIssues();
	GenerateRecommendations();
}

ApplicationHealthMonitor::HealthStatus ApplicationHealthMonitor::GetHealthStatus() const {
	return currentStatus;
}

float ApplicationHealthMonitor::GetHealthScore() const {
	return healthScore;
}

std::string ApplicationHealthMonitor::GetHealthReport() const {
	std::stringstream report;
	report << "Application Health Report\n";
	report << "========================\n";
	report << "Status: ";

	switch (currentStatus) {
	case HealthStatus::EXCELLENT:
		report << "Excellent";
		break;
	case HealthStatus::GOOD:
		report << "Good";
		break;
	case HealthStatus::FAIR:
		report << "Fair";
		break;
	case HealthStatus::POOR:
		report << "Poor";
		break;
	case HealthStatus::CRITICAL:
		report << "Critical";
		break;
	}

	report << "\nHealth Score: " << healthScore << "/100\n";
	report << "\nIssues:\n";
	for (const auto& issue : currentIssues) {
		report << "- " << issue << "\n";
	}

	report << "\nRecommendations:\n";
	for (const auto& rec : recommendations) {
		report << "- " << rec << "\n";
	}

	return report.str();
}

std::vector<std::string> ApplicationHealthMonitor::GetIssues() const {
	return currentIssues;
}

std::vector<std::string> ApplicationHealthMonitor::GetRecommendations() const {
	return recommendations;
}

bool ApplicationHealthMonitor::IsHealthy() const {
	return currentStatus == HealthStatus::EXCELLENT || currentStatus == HealthStatus::GOOD;
}

void ApplicationHealthMonitor::AnalyzeHealth() {
	PerformanceMetrics metrics = performanceMonitor.GetMetrics();
	healthScore = 100.0f;

	// Анализируем CPU использование
	if (metrics.cpuUsagePercent > 90) {
		healthScore -= 30;
	} else if (metrics.cpuUsagePercent > 70) {
		healthScore -= 15;
	}

	// Анализируем память
	if (metrics.memoryUsageMB > metrics.maxMemoryMB * 0.9f) {
		healthScore -= 25;
	} else if (metrics.memoryUsageMB > metrics.maxMemoryMB * 0.7f) {
		healthScore -= 10;
	}

	// Анализируем ошибки
	if (metrics.totalErrors > 100) {
		healthScore -= 20;
	}

	// Определяем статус
	if (healthScore >= 90) {
		currentStatus = HealthStatus::EXCELLENT;
	} else if (healthScore >= 75) {
		currentStatus = HealthStatus::GOOD;
	} else if (healthScore >= 60) {
		currentStatus = HealthStatus::FAIR;
	} else if (healthScore >= 40) {
		currentStatus = HealthStatus::POOR;
	} else {
		currentStatus = HealthStatus::CRITICAL;
	}
}

void ApplicationHealthMonitor::IdentifyIssues() {
	currentIssues.clear();
	PerformanceMetrics metrics = performanceMonitor.GetMetrics();

	if (metrics.cpuUsagePercent > 90) {
		currentIssues.push_back("High CPU usage: " + std::to_string((int)metrics.cpuUsagePercent) + "%");
	}

	if (metrics.memoryUsageMB > metrics.maxMemoryMB * 0.9f) {
		currentIssues.push_back("High memory usage");
	}

	if (metrics.totalErrors > 50) {
		currentIssues.push_back("High error count: " + std::to_string(metrics.totalErrors));
	}
}

void ApplicationHealthMonitor::GenerateRecommendations() {
	recommendations.clear();

	if (!currentIssues.empty()) {
		for (const auto& issue : currentIssues) {
			if (issue.find("CPU") != std::string::npos) {
				recommendations.push_back("Consider pausing some downloads to reduce CPU load");
			}
			if (issue.find("memory") != std::string::npos) {
				recommendations.push_back("Clear cache or reduce download queue");
			}
			if (issue.find("error") != std::string::npos) {
				recommendations.push_back("Check logs for detailed error information");
			}
		}
	}
}
