#include "SettingsManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windows.h>

SettingsManager::SettingsManager() {
	SetDefaults();
}

SettingsManager::~SettingsManager() {
}

void SettingsManager::SetDefaults() {
	settings.downloadPath = "./Downloads";
	settings.maxConnections = 100;
	settings.maxUploadSpeed = 0;      // Без ограничений
	settings.maxDownloadSpeed = 0;    // Без ограничений
	settings.maxCacheSize = 512;      // 512 MB
	settings.enableUpload = true;
	settings.enableDHT = true;
	settings.enablePEX = true;
	settings.listeningPort = 6881;
	settings.language = "English";
}

bool SettingsManager::LoadSettings(const std::string& configPath) {
	std::ifstream file(configPath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		size_t pos = line.find('=');
		if (pos == std::string::npos) continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		// Удаляем пробелы
		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);

		if (key == "DownloadPath") settings.downloadPath = value;
		else if (key == "MaxConnections") settings.maxConnections = std::stoul(value);
		else if (key == "MaxUploadSpeed") settings.maxUploadSpeed = std::stoul(value);
		else if (key == "MaxDownloadSpeed") settings.maxDownloadSpeed = std::stoul(value);
		else if (key == "MaxCacheSize") settings.maxCacheSize = std::stoul(value);
		else if (key == "EnableUpload") settings.enableUpload = (value == "true");
		else if (key == "EnableDHT") settings.enableDHT = (value == "true");
		else if (key == "EnablePEX") settings.enablePEX = (value == "true");
		else if (key == "ListeningPort") settings.listeningPort = std::stoul(value);
		else if (key == "Language") settings.language = value;
	}

	file.close();
	return true;
}

bool SettingsManager::SaveSettings(const std::string& configPath) {
	std::ofstream file(configPath);
	if (!file.is_open()) {
		return false;
	}

	file << "DownloadPath = " << settings.downloadPath << "\n";
	file << "MaxConnections = " << settings.maxConnections << "\n";
	file << "MaxUploadSpeed = " << settings.maxUploadSpeed << "\n";
	file << "MaxDownloadSpeed = " << settings.maxDownloadSpeed << "\n";
	file << "MaxCacheSize = " << settings.maxCacheSize << "\n";
	file << "EnableUpload = " << (settings.enableUpload ? "true" : "false") << "\n";
	file << "EnableDHT = " << (settings.enableDHT ? "true" : "false") << "\n";
	file << "EnablePEX = " << (settings.enablePEX ? "true" : "false") << "\n";
	file << "ListeningPort = " << settings.listeningPort << "\n";
	file << "Language = " << settings.language << "\n";

	file.close();
	return true;
}

AppSettings SettingsManager::GetSettings() const {
	return settings;
}

void SettingsManager::SetSettings(const AppSettings& newSettings) {
	settings = newSettings;
}

void SettingsManager::SetDownloadPath(const std::string& path) {
	settings.downloadPath = path;
}

void SettingsManager::SetMaxConnections(uint32_t maxConn) {
	settings.maxConnections = maxConn;
}

void SettingsManager::SetMaxUploadSpeed(uint32_t speedKBps) {
	settings.maxUploadSpeed = speedKBps;
}

void SettingsManager::SetMaxDownloadSpeed(uint32_t speedKBps) {
	settings.maxDownloadSpeed = speedKBps;
}

void SettingsManager::SetMaxCacheSize(uint32_t sizeMB) {
	settings.maxCacheSize = sizeMB;
}

void SettingsManager::SetListeningPort(uint16_t port) {
	settings.listeningPort = port;
}

// StatisticsManager
StatisticsManager::StatisticsManager() {
	stats.totalDownloaded = 0;
	stats.totalUploaded = 0;
	stats.totalDownloadedSessions = 0;
	stats.totalUploadedSessions = 0;
	stats.completedTorrents = 0;
	stats.totalActivePeers = 0;
	stats.averageDownloadSpeed = 0.0f;
	stats.averageUploadSpeed = 0.0f;
}

StatisticsManager::~StatisticsManager() {
}

bool StatisticsManager::LoadStatistics(const std::string& statsPath) {
	std::ifstream file(statsPath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		size_t pos = line.find('=');
		if (pos == std::string::npos) continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);

		if (key == "TotalDownloaded") stats.totalDownloaded = std::stoull(value);
		else if (key == "TotalUploaded") stats.totalUploaded = std::stoull(value);
		else if (key == "CompletedTorrents") stats.completedTorrents = std::stoul(value);
	}

	file.close();
	return true;
}

bool StatisticsManager::SaveStatistics(const std::string& statsPath) {
	std::ofstream file(statsPath);
	if (!file.is_open()) {
		return false;
	}

	file << "TotalDownloaded = " << stats.totalDownloaded << "\n";
	file << "TotalUploaded = " << stats.totalUploaded << "\n";
	file << "CompletedTorrents = " << stats.completedTorrents << "\n";

	file.close();
	return true;
}

void StatisticsManager::AddDownloadedBytes(uint64_t bytes) {
	stats.totalDownloaded += bytes;
	stats.totalDownloadedSessions += bytes;
}

void StatisticsManager::AddUploadedBytes(uint64_t bytes) {
	stats.totalUploaded += bytes;
	stats.totalUploadedSessions += bytes;
}

void StatisticsManager::AddCompletedTorrent() {
	stats.completedTorrents++;
}

void StatisticsManager::UpdateActivePeers(uint32_t count) {
	stats.totalActivePeers = count;
}

void StatisticsManager::UpdateDownloadSpeed(float speedKBps) {
	stats.averageDownloadSpeed = speedKBps;
}

void StatisticsManager::UpdateUploadSpeed(float speedKBps) {
	stats.averageUploadSpeed = speedKBps;
}

AppStatistics StatisticsManager::GetStatistics() const {
	return stats;
}

void StatisticsManager::ResetSessionStats() {
	stats.totalDownloadedSessions = 0;
	stats.totalUploadedSessions = 0;
}
