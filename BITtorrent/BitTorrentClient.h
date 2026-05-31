#pragma once

#include "TorrentCore.h"
#include "DownloadManager.h"
#include "TrackerManager.h"
#include "StorageManager.h"
#include "CacheManager.h"
#include "SettingsManager.h"
#include "Logger.h"
#include <vector>
#include <memory>
#include <string>

// Основной менеджер приложения, который координирует все компоненты
class BitTorrentClient {
public:
	BitTorrentClient();
	~BitTorrentClient();

	// Инициализация
	bool Initialize(const std::string& configPath);
	bool Shutdown();

	// Управление торрентами
	bool LoadTorrent(const std::string& torrentPath, TorrentInfo& outTorrent);
	bool StartDownload(const TorrentInfo& torrent);
	bool PauseDownload(const std::string& torrentName);
	bool ResumeDownload(const std::string& torrentName);
	bool StopDownload(const std::string& torrentName);
	bool RemoveTorrent(const std::string& torrentName);

	// Информация о торрентах
	std::vector<TorrentInfo> GetActiveTorrents() const;
	std::vector<DownloadHistoryEntry> GetDownloadHistory() const;

	// Статистика
	AppStatistics GetStatistics() const;
	void ResetStatistics();

	// Настройки
	bool LoadSettings();
	bool SaveSettings();
	AppSettings GetSettings() const;
	void UpdateSettings(const AppSettings& newSettings);

	// Менеджеры для доступа
	DownloadManager& GetDownloadManager(const std::string& torrentName);
	TrackerManager& GetTrackerManager() { return trackerManager; }
	StorageManager& GetStorageManager() { return storageManager; }
	CacheManager& GetCacheManager() { return cacheManager; }
	SettingsManager& GetSettingsManager() { return settingsManager; }
	StatisticsManager& GetStatisticsManager() { return statisticsManager; }
	Logger& GetLogger() { return Logger::GetInstance(); }

private:
	std::vector<std::pair<std::string, std::unique_ptr<DownloadManager>>> activeTorrents;

	TrackerManager trackerManager;
	StorageManager storageManager;
	CacheManager cacheManager;
	SettingsManager settingsManager;
	StatisticsManager statisticsManager;

	std::string configPath;
	bool initialized;

	DownloadManager* FindDownloadManager(const std::string& torrentName);
};
