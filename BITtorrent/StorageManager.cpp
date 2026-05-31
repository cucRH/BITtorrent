#include "StorageManager.h"
#include <fstream>
#include <sstream>
#include <windows.h>

StorageManager::StorageManager()
	: basePath("./BitTorrent_Data"),
	historyFile("./BitTorrent_Data/history.txt") {
}

StorageManager::~StorageManager() {
}

bool StorageManager::Initialize(const std::string& storagePath) {
	basePath = storagePath;
	historyFile = storagePath + "/history.txt";

	// Создаем директорию хранилища если её нет
	CreateDirectoryA(basePath.c_str(), nullptr);
	return true;
}

bool StorageManager::SaveDownloadHistory(const std::vector<DownloadHistoryEntry>& history) {
	std::ofstream file(historyFile);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& entry : history) {
		file << entry.torrentName << "|"
			 << entry.filePath << "|"
			 << entry.totalSize << "|"
			 << entry.downloadedSize << "|"
			 << entry.status << "|"
			 << entry.dateAdded << "|"
			 << entry.dateCompleted << "\n";
	}

	file.close();
	return true;
}

std::vector<DownloadHistoryEntry> StorageManager::LoadDownloadHistory() const {
	std::vector<DownloadHistoryEntry> history;
	std::ifstream file(historyFile);

	if (!file.is_open()) {
		return history;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;
		DownloadHistoryEntry entry = {};

		std::getline(ss, entry.torrentName, '|');
		std::getline(ss, entry.filePath, '|');

		std::getline(ss, token, '|');
		entry.totalSize = std::stoull(token);

		std::getline(ss, token, '|');
		entry.downloadedSize = std::stoull(token);

		std::getline(ss, entry.status, '|');

		std::getline(ss, token, '|');
		entry.dateAdded = std::stoull(token);

		std::getline(ss, token, '|');
		entry.dateCompleted = std::stoull(token);

		history.push_back(entry);
	}

	file.close();
	return history;
}

bool StorageManager::SaveTorrentMetadata(const TorrentInfo& torrent) {
	std::string metadataDir = basePath + "/metadata";
	CreateDirectoryA(metadataDir.c_str(), nullptr);

	std::string metadataFile = metadataDir + "/" + torrent.name + ".meta";
	std::ofstream file(metadataFile);
	if (!file.is_open()) {
		return false;
	}

	file << "Name: " << torrent.name << "\n"
		 << "Announce: " << torrent.announce << "\n"
		 << "TotalLength: " << torrent.totalLength << "\n"
		 << "PieceLength: " << torrent.pieceLength << "\n"
		 << "Comment: " << torrent.comment << "\n";

	file.close();
	return true;
}

TorrentInfo StorageManager::LoadTorrentMetadata(const std::string& torrentName) {
	TorrentInfo torrent = {};
	std::string metadataFile = basePath + "/metadata/" + torrentName + ".meta";
	std::ifstream file(metadataFile);

	if (!file.is_open()) {
		return torrent;
	}

	std::string line;
	while (std::getline(file, line)) {
		size_t pos = line.find(": ");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);

			if (key == "Name") torrent.name = value;
			else if (key == "Announce") torrent.announce = value;
			else if (key == "TotalLength") torrent.totalLength = std::stoull(value);
			else if (key == "PieceLength") torrent.pieceLength = std::stoul(value);
			else if (key == "Comment") torrent.comment = value;
		}
	}

	file.close();
	return torrent;
}

std::string StorageManager::GetTorrentDataPath(const std::string& torrentName) {
	return basePath + "/" + torrentName;
}

bool StorageManager::CreateTorrentDirectory(const std::string& torrentName) {
	std::string path = GetTorrentDataPath(torrentName);
	return CreateDirectoryA(path.c_str(), nullptr) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}
