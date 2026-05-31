#pragma once

#include <string>
#include <vector>
#include "TorrentCore.h"

// Информация о загрузке в истории
struct DownloadHistoryEntry {
	std::string torrentName;
	std::string filePath;
	uint64_t totalSize;
	uint64_t downloadedSize;
	std::string status;
	uint64_t dateAdded;
	uint64_t dateCompleted;
};

// Менеджер хранилища для управления информацией о загрузках
class StorageManager {
public:
	StorageManager();
	~StorageManager();

	bool Initialize(const std::string& storagePath);

	bool SaveDownloadHistory(const std::vector<DownloadHistoryEntry>& history);
	std::vector<DownloadHistoryEntry> LoadDownloadHistory() const;

	bool SaveTorrentMetadata(const TorrentInfo& torrent);
	TorrentInfo LoadTorrentMetadata(const std::string& torrentName);

	std::string GetTorrentDataPath(const std::string& torrentName);
	bool CreateTorrentDirectory(const std::string& torrentName);

private:
	std::string basePath;
	std::string historyFile;
};
