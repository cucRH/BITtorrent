#pragma once

#include <string>
#include <vector>
#include <cstdint>

// Структура для хранения настроек приложения
struct AppSettings {
	std::string downloadPath;
	uint32_t maxConnections;
	uint32_t maxUploadSpeed;     // в KB/s
	uint32_t maxDownloadSpeed;   // в KB/s
	uint32_t maxCacheSize;       // в MB
	bool enableUpload;
	bool enableDHT;
	bool enablePEX;
	uint16_t listeningPort;
	std::string language;
};

// Класс для управления настройками
class SettingsManager {
public:
	SettingsManager();
	~SettingsManager();

	bool LoadSettings(const std::string& configPath);
	bool SaveSettings(const std::string& configPath);

	AppSettings GetSettings() const;
	void SetSettings(const AppSettings& settings);

	// Отдельные методы для установки значений
	void SetDownloadPath(const std::string& path);
	void SetMaxConnections(uint32_t maxConn);
	void SetMaxUploadSpeed(uint32_t speedKBps);
	void SetMaxDownloadSpeed(uint32_t speedKBps);
	void SetMaxCacheSize(uint32_t sizeMB);
	void SetListeningPort(uint16_t port);

private:
	AppSettings settings;
	void SetDefaults();
};

// Статистика приложения
struct AppStatistics {
	uint64_t totalDownloaded;
	uint64_t totalUploaded;
	uint64_t totalDownloadedSessions;
	uint64_t totalUploadedSessions;
	uint32_t completedTorrents;
	uint32_t totalActivePeers;
	float averageDownloadSpeed;
	float averageUploadSpeed;
};

// Класс для отслеживания статистики
class StatisticsManager {
public:
	StatisticsManager();
	~StatisticsManager();

	bool LoadStatistics(const std::string& statsPath);
	bool SaveStatistics(const std::string& statsPath);

	void AddDownloadedBytes(uint64_t bytes);
	void AddUploadedBytes(uint64_t bytes);
	void AddCompletedTorrent();
	void UpdateActivePeers(uint32_t count);
	void UpdateDownloadSpeed(float speedKBps);
	void UpdateUploadSpeed(float speedKBps);

	AppStatistics GetStatistics() const;
	void ResetSessionStats();

private:
	AppStatistics stats;
};
